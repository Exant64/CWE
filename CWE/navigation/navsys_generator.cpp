#include "stdafx.h"

#include <ninja_functions.h>
#include <ChaoMain.h>
#include <al_stage.h>

#include "navsys_generator.h"
#include "navsys_meshconvert.h"
#include "navsys_log.h"
#include "navsys.h"

#include "external/Detour/Include/DetourNavMeshBuilder.h"
#include "external/Detour/Include/DetourNavMeshQuery.h"
#include "external/Recast/Include/RecastAlloc.h"

#include <assert.h>

#ifdef MEMORY_PROFILE
    #include <memory_debug.h>
#endif

// custom deleter for unique_ptrs in thread
template <typename T>
struct RcDeleter {
    void operator()(T* ptr) const {
        // copied from rcDelete since it's not externd
        if (ptr) {
            ptr->~T();
            rcFree((void*)ptr);
        }
    }
};

// custom deleter for navmesh
struct DtDeleter {
    void operator()(dtNavMesh* ptr) {
        if(ptr) dtFreeNavMesh(ptr);
    }
};

NavSysGenerator gNavSysGenerator;

struct NavMeshSetHeader {
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

constexpr int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T';  //'MSET';
constexpr int NAVMESHSET_VERSION = 1;

void NavSysGenerator::SaveNavMesh(const char* path, const dtNavMesh* mesh) {
	if (!mesh)
	{
		return;
	}

	FILE* file = fopen(path, "wb");
	if (!file)
	{
		return;
	}

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize)
		{
			continue;
		}
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, file);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize)
		{
			continue;
		}

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, file);

		fwrite(tile->data, tile->dataSize, 1, file);
	}

	fclose(file);
}

dtNavMesh* NavSysGenerator::LoadNavMesh(const char* path) {
	FILE* file = fopen(path, "rb");

	if (!file) {
		return nullptr;
	}

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, file);
	if (readLen != 1)
	{
		fclose(file);
		return nullptr;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(file);
		return nullptr;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(file);
		return nullptr;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(file);
		return nullptr;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(file);
		return nullptr;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, file);
		if (readLen != 1)
		{
			fclose(file);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
		{
			break;
		}

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data)
		{
			break;
		}
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, file);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(file);
			return 0;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(file);

	return mesh;
}

#ifdef MEMORY_PROFILE 
    static MemoryProfiler memProfiler;

    void* customAlloc (size_t size, rcAllocHint) {
        return memProfiler.alloc(size);
    }

    void customFree (void* ptr) {
        memProfiler.free(ptr);
    };
#endif

void NavSysGenerator::PopulateClimbSpots(std::vector<NavSysGenerator::ClimbSpot>& spots) const {
    task* pTask = ObjectLists[2];

    if(!pTask) {
        return;
    }

    do {
        // C_CLIMB executor pointer
        if(pTask->MainSub != ObjectFuncPtr(0x55AB20)) {
            pTask = pTask->NextObject;
            continue;
        }

        auto work = pTask->Data1.Entity;
        
        if(!work || !work->Collision || !work->Collision->CollisionArray) {
            pTask = pTask->NextObject;
            continue;
        }

        auto colliEntry = work->Collision->CollisionArray;

        // anonymous_x = xyz scale
        ClimbSpot spot = {
            .m_pos = work->Position,
            .m_extent = { 
                njAbs(colliEntry->anonymous_1),
                njAbs(colliEntry->anonymous_2), 
                njAbs(colliEntry->anonymous_3) 
            },
            .m_inverseAngY = -work->Rotation.y,
            .m_radiusSquared = work->Collision->field_8 * work->Collision->field_8
        };

        spots.push_back(spot);

        pTask = pTask->NextObject;
    } while(pTask && pTask != ObjectLists[2]);
}

bool NavSysGenerator::CheckIfPointInsideAnyClimbSpot(const std::vector<ClimbSpot>& spots, const NJS_POINT3& p) const {
    for(const auto& spot : spots) {
        NJS_POINT3 checkP = {
            p.x - spot.m_pos.x,
            p.y - spot.m_pos.y,
            p.z - spot.m_pos.z
        };

        // check if in collision radius first
        if(njInnerProduct(&checkP, &checkP) > spot.m_radiusSquared) continue;

        njPushUnitMatrix();
        njRotateY(_nj_current_matrix_ptr_, spot.m_inverseAngY);
        // no translation so we can use the "float4(x, 0)" type of transform
        sub_426CC0(_nj_current_matrix_ptr_, &checkP, &checkP, 1);
        njPopMatrixEx();

        const bool obbCheck = njAbs(checkP.x) <= spot.m_extent.x &&
            njAbs(checkP.y) <= spot.m_extent.y && 
            njAbs(checkP.z) <= spot.m_extent.z;
        
        if(obbCheck) {
            return true;
        }
    }

    return false;
}

std::vector<NJS_POINT3> NavSysGenerator::GenerateOffMeshClimbSpots(rcHeightfield* solid, const int walkableRadius, const std::vector<ClimbSpot>& spots) const {
    std::vector<NJS_POINT3> offmesh;

    for (int z = 0; z < solid->height; ++z) {
        for (int x = 0; x < solid->width; ++x) {
            rcSpan* span = solid->spans[x + z * solid->width];
            while (span) {
                // if we're not in water,
                // OR we are, but the next area is water too
                // skip em (on multiple water spans on top of eachother, we only wanna calc climb with the top one)
                if(span->area != NAV_AREA_WATER || (span->next && span->next->area == NAV_AREA_WATER)) {
                    span = span->next;
                    continue;
                }

                for (int direction = 0; direction < 4; ++direction) {
                    const int dirX = rcGetDirOffsetX(direction);
                    const int dirZ = rcGetDirOffsetY(direction);
                    const int neighborX = x + dirX;
                    const int neighborZ = z + dirZ;

                    if (neighborX < 0 || neighborZ < 0 || neighborX >= solid->width || neighborZ >= solid->height) {
                        break;
                    }

                    rcSpan* neighborSpan = solid->spans[neighborX + neighborZ * solid->width];
                    while(neighborSpan) {
                        // if the water surface (smax) isn't in the interval of the neighboring ground, then invalid
                        if(neighborSpan->area != RC_WALKABLE_AREA || neighborSpan->smin >= span->smin || neighborSpan->smax < span->smax) {
                            neighborSpan = neighborSpan->next;
                            continue;
                        }

                        // sometimes water geometry spans past where it visually ends, and tries to connect to ground next to it
                        // this is usually below the actual walkable geo so we try to filter those out here
                        // basically if floor above is in the interval of the climbspot then the chao would hit it's head, so it's invalid
                        if(span->next) {
                            if(neighborSpan->smin <= span->next->smax && neighborSpan->smax >= span->next->smax) {
                                neighborSpan = neighborSpan->next;
                                continue;
                            }
                        }

                        // filter out any obviously too high walls
                        // (EC garden walls recognized as climb because SADX gardens are covered in one big CCLIMB)
                        if(njAbs(float(span->smax) - float(neighborSpan->smax)) * solid->ch > 250) {
                            neighborSpan = neighborSpan->next;
                            continue;
                        }

                        if(!CheckIfPointInsideAnyClimbSpot(spots, {
                            solid->bmin[0] + (float(x) + 0.5f) * solid->cs,
                            solid->bmin[1] + (float(span->smin) + 0.5f) * solid->ch,
                            solid->bmin[2] + (float(z) + 0.5f) * solid->cs
                        })) {
                            neighborSpan = neighborSpan->next;
                            continue;
                        }

                        offmesh.push_back({
                            solid->bmin[0] + (float(x) + 0.5f) * solid->cs,
                            solid->bmin[1] + (float(span->smin) + 0.5f) * solid->ch,
                            solid->bmin[2] + (float(z) + 0.5f) * solid->cs
                        });
                        offmesh.push_back({
                            solid->bmin[0] + (float(neighborX) + 0.5f) * solid->cs,
                            solid->bmin[1] + (float(neighborSpan->smax) + 0.5f) * solid->ch,
                            solid->bmin[2] + (float(neighborZ) + 0.5f) * solid->cs
                        });
                        #if 0
                        offmesh.push_back({
                            solid->bmin[0] + (float(x - dirX * (walkableRadius * 2.5f)) + 0.5f) * solid->cs,
                            solid->bmin[1] + (float(span->smin) + 0.5f) * solid->ch,
                            solid->bmin[2] + (float(z - dirZ * (walkableRadius * 2.5f)) + 0.5f) * solid->cs
                        });

                        offmesh.push_back({
                            solid->bmin[0] + (float(neighborX + dirX * (walkableRadius * 2.5f)) + 0.5f) * solid->cs,
                            solid->bmin[1] + (float(neighborSpan->smax) + 0.5f) * solid->ch,
                            solid->bmin[2] + (float(neighborZ + dirZ * (walkableRadius * 2.5f)) + 0.5f) * solid->cs
                        });
#endif
                        break;                      
                    }
                }

                span = span->next;
            }
        }
    }

    return offmesh;
}

std::shared_ptr<dtNavMesh> NavSysGenerator::TryLoad(const uint32_t hash) {
    if(!m_useCache) {
        return NULL;
    }

    char land_navmesh_path[40];
    sprintf_s(land_navmesh_path, "cwe_nav_%x", hash);

    // check if cached file already exists before spinning up thread
    auto tryLoad = LoadNavMesh(land_navmesh_path);
    if(tryLoad) {
        return std::shared_ptr<dtNavMesh>(tryLoad, DtDeleter {});
    }

    return NULL;
}

// set ground below water to water aswell to prevent walking "below water"
void NavSysGenerator::AssignWaterToGroundBelowWater(rcHeightfield* solid) {
    for (int z = 0; z < solid->height; ++z) {
        for (int x = 0; x < solid->width; ++x) {
            rcSpan* span = solid->spans[x + z * solid->width];
            rcSpan* prev = nullptr;

            // spans are like lists of a cell at a given XZ pos
            // each element is a cell from bottom to top
            // so if we detect water, the previous cell was the ground below it
            while (span) {
                if (span->area == NAV_AREA_WATER && prev) {
                    prev->area = NAV_AREA_WATER;
                }

                prev = span;
                span = span->next;
            }
        }
    }
}

std::future<std::shared_ptr<dtNavMesh>> NavSysGenerator::TryGenerate(const uint32_t hash) {
    // I rather do the mesh conversion not multithreaded, shouldnt be too expensive
    NavSysMeshConvert mesh;

    // same here
    std::vector<ClimbSpot> spots;
    PopulateClimbSpots(spots);

    return std::async(std::launch::async, [this, m_mesh{std::move(mesh)}, hash, spots{std::move(spots)}] {
        #ifdef MEMORY_PROFILE
            memProfiler.clear();
            
            rcAllocSetCustom(customAlloc, customFree);
        #endif

        #ifdef TIME_PROFILE
            LARGE_INTEGER startTime;
            QueryPerformanceCounter(&startTime);
        #endif

        char land_navmesh_path[40];
        sprintf_s(land_navmesh_path, "cwe_nav_%x", hash);

        NavSysLog("Started navmesh generation: %s", land_navmesh_path);

        cweRcContext m_recastContext;

        std::shared_ptr<dtNavMesh> result = NULL;

        const float* verts = &m_mesh.getVerts().data()->x;
        const int nverts = m_mesh.getVertCount();
        const int* tris = m_mesh.getTris().data();
        const int ntris = m_mesh.getTriCount();

        float bmin[3];
        float bmax[3];
        rcCalcBounds(verts, nverts, bmin, bmax);

        const int walkableHeight = (int)ceilf(m_config.m_agentHeight / m_config.m_cellHeight);
        const int walkableClimb = (int)floorf(m_config.m_agentMaxClimb / m_config.m_cellHeight);
        const int walkableRadius = (int)ceilf(m_config.m_agentRadius / m_config.m_cellSize);
        const int maxEdgeLen = (int)(m_config.m_edgeMaxLen / m_config.m_cellSize);
        const int minRegionArea = (int)rcSqr(m_config.m_regionMinSize);		// Note: area = size*size
        const int mergeRegionArea = (int)rcSqr(m_config.m_regionMergeSize);	// Note: area = size*size
        const float detailSampleDist = (m_config.m_detailSampleDist < 0.9f) ? 0 : m_config.m_cellSize * m_config.m_detailSampleDist;
        const float detailSampleMaxError = m_config.m_cellHeight * m_config.m_detailSampleMaxError;

        int width, height;

        // Set the area where the navigation will be build.
        // Here the bounds of the input mesh are used, but the
        // area could be specified by an user defined box, etc.
        rcCalcGridSize(bmin, bmax, m_config.m_cellSize, &width, &height);

        //
        // Step 2. Rasterize input polygon soup.
        //

        // Allocate voxel heightfield where we rasterize our input data to.
        std::unique_ptr<rcHeightfield, RcDeleter<rcHeightfield>> solid { rcAllocHeightfield() };
        if (!solid)
        {
            NavSysLog("buildNavigation: Out of memory 'solid'.");
            return result;
        }
        if (!rcCreateHeightfield(&m_recastContext, *solid, width, height, bmin, bmax, m_config.m_cellSize, m_config.m_cellHeight))
        {
            NavSysLog("buildNavigation: Could not create solid heightfield.");
            return result;
        }

        // calc span areas
        {
            std::unique_ptr<uint8_t> m_triareas { new unsigned char[ntris]};
            if (!m_triareas)
            {
                NavSysLog("buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
                return result;
            }

            // Find triangles which are walkable based on their slope and rasterize them.
            // If your input data is multiple meshes, you can transform them here, calculate
            // the are type for each of the meshes and rasterize them.

            assert(m_mesh.getAreas().size() == ntris);

            memset(m_triareas.get(), 0, sizeof(unsigned char) * ntris);

            rcMarkWalkableTriangles(&m_recastContext, m_config.m_agentMaxSlope, verts, nverts, tris, ntris, m_triareas.get());

            // overwrite custom areas
            for(size_t i = 0; i < m_mesh.getAreas().size(); ++i) {
                if(m_mesh.getAreas()[i] == NAV_AREA_WATER) {
                    m_triareas.get()[i] = NAV_AREA_WATER;
                }
            }

            if (!rcRasterizeTriangles(&m_recastContext, verts, nverts, tris, m_triareas.get(), ntris, *solid, walkableClimb))
            {
                NavSysLog("buildNavigation: Could not rasterize triangles.");
                return result;
            }
        }

        AssignWaterToGroundBelowWater(solid.get());

        //
        // Step 3. Filter walkable surfaces.
        //

        // Once all geometry is rasterized, we do initial pass of filtering to
        // remove unwanted overhangs caused by the conservative rasterization
        // as well as filter spans where the character cannot possibly stand.
        if (m_config.m_filterLowHanging)
            rcFilterLowHangingWalkableObstacles(&m_recastContext, walkableClimb, *solid);

        if (m_config.m_filterWalkableLowHeightSpans)
            rcFilterWalkableLowHeightSpans(&m_recastContext, walkableHeight, *solid);
    
        std::vector<NJS_POINT3> offmesh = GenerateOffMeshClimbSpots(solid.get(), walkableRadius, spots);

        if (m_config.m_filterLedgeSpans)
            rcFilterLedgeSpans(&m_recastContext, walkableHeight, walkableClimb, *solid);

        //
        // Step 4. Partition walkable surface to simple regions.
        //

        // Compact the heightfield so that it is faster to handle from now on.
        // This will result more cache coherent data as well as the neighbours
        // between walkable cells will be calculated.
        std::unique_ptr<rcCompactHeightfield, RcDeleter<rcCompactHeightfield>> chf { rcAllocCompactHeightfield() };
        if (!chf)
        {
            NavSysLog("buildNavigation: Out of memory 'chf'.");
            return result;
        }
        if (!rcBuildCompactHeightfield(&m_recastContext, walkableHeight, walkableClimb, *solid, *chf))
        {
            NavSysLog("buildNavigation: Could not build compact data.");
            return result;
        }

        solid = NULL;
        
        // Erode the walkable area by agent radius.
        if (!rcErodeWalkableArea(&m_recastContext, walkableRadius, *chf))
        {
            NavSysLog("buildNavigation: Could not erode.");
            return result;
        }

        // (Optional) Mark areas.
        #if 0
            const ConvexVolume* vols = m_geom->getConvexVolumes();
            for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
                rcMarkConvexPolyArea(&m_recastContext, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);
        #endif

        // we ended up picking monotone type generation for speed
		if (!rcBuildRegionsMonotone(&m_recastContext, *chf, 0, minRegionArea, mergeRegionArea))
		{
			NavSysLog("buildNavigation: Could not build monotone regions.");
			return result;
		}

        //
        // Step 5. Trace and simplify region contours.
        //

        // Create contours.
        std::unique_ptr<rcContourSet, RcDeleter<rcContourSet>> cset { rcAllocContourSet() };
        if (!cset)
        {
            NavSysLog("buildNavigation: Out of memory 'cset'.");
            return result;
        }
        if (!rcBuildContours(&m_recastContext, *chf, m_config.m_edgeMaxError, maxEdgeLen, *cset))
        {
            NavSysLog("buildNavigation: Could not create contours.");
            return result;
        }

        //
        // Step 6. Build polygons mesh from contours.
        //

        // Build polygon navmesh from the contours.
        std::unique_ptr<rcPolyMesh, RcDeleter<rcPolyMesh>> pmesh { rcAllocPolyMesh() };
        if (!pmesh)
        {
            NavSysLog("buildNavigation: Out of memory 'pmesh'.");
            return result;
        }
        if (!rcBuildPolyMesh(&m_recastContext, *cset, m_config.m_vertsPerPoly, *pmesh))
        {
            NavSysLog("buildNavigation: Could not triangulate contours.");
            return result;
        }

        //
        // Step 7. Create detail mesh which allows to access approximate height on each polygon.
        //

        std::unique_ptr<rcPolyMeshDetail, RcDeleter<rcPolyMeshDetail>> dmesh { rcAllocPolyMeshDetail() };
        if (!dmesh)
        {
            NavSysLog("buildNavigation: Out of memory 'pmdtl'.");
            return result;
        }

        if (!rcBuildPolyMeshDetail(&m_recastContext, *pmesh, *chf, detailSampleDist, detailSampleMaxError, *dmesh))
        {
            NavSysLog("buildNavigation: Could not build detail mesh.");
            return result;
        }

        chf = NULL;
        cset = NULL;

        // At this point the navigation mesh data is ready, you can access it from m_pmesh.
        // See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

        //
        // (Optional) Step 8. Create Detour data from Recast poly mesh.
        //

        // The GUI may allow more max points per polygon than Detour can handle.
        // Only build the detour navmesh if we do not exceed the limit.
        if (m_config.m_vertsPerPoly <= DT_VERTS_PER_POLYGON)
        {
            unsigned char* navData = 0;
            int navDataSize = 0;

            // Update poly flags from areas.
            for (int i = 0; i < pmesh->npolys; ++i) {
                pmesh->flags[i] = NAV_FLAGS_WALK;

                if (pmesh->areas[i] == RC_WALKABLE_AREA)
                    pmesh->areas[i] = NAV_AREA_GROUND;
                
                if (pmesh->areas[i] == NAV_AREA_GROUND) {
                    pmesh->flags[i] = NAV_FLAGS_WALK;
                }
                else if (pmesh->areas[i] == NAV_AREA_WATER) {
                    pmesh->flags[i] = NAV_FLAGS_SWIM;
                }
            }
                
            dtNavMeshCreateParams params;
            memset(&params, 0, sizeof(params));
            params.verts = pmesh->verts;
            params.vertCount = pmesh->nverts;
            params.polys = pmesh->polys;
            params.polyAreas = pmesh->areas;
            params.polyFlags = pmesh->flags;
            params.polyCount = pmesh->npolys;
            params.nvp = pmesh->nvp;
            params.detailMeshes = dmesh->meshes;
            params.detailVerts = dmesh->verts;
            params.detailVertsCount = dmesh->nverts;
            params.detailTris = dmesh->tris;
            params.detailTriCount = dmesh->ntris;

            std::vector<float> rad;
            std::vector<uint8_t> dir;
            std::vector<uint8_t> areas;
            std::vector<uint16_t> flags;
            std::vector<uint32_t> userIDs;

            for(size_t i = 0; i < offmesh.size() / 2; i++) {
                rad.push_back(m_config.m_agentRadius * 2);
                dir.push_back(DT_OFFMESH_CON_BIDIR);
                areas.push_back(NAV_AREA_GROUND);
                flags.push_back(NAV_FLAGS_WALK);
                userIDs.push_back(1000 + i);
            }
            
            params.offMeshConVerts = &offmesh.data()->x;
            params.offMeshConRad = rad.data();
            params.offMeshConDir = dir.data();
            params.offMeshConAreas = areas.data();
            params.offMeshConFlags = flags.data();
            params.offMeshConUserID = userIDs.data();
            params.offMeshConCount = flags.size();

            params.walkableHeight = m_config.m_agentHeight;
            params.walkableRadius = m_config.m_agentRadius;
            params.walkableClimb = m_config.m_agentMaxClimb;
            rcVcopy(params.bmin, pmesh->bmin);
            rcVcopy(params.bmax, pmesh->bmax);
            params.cs = m_config.m_cellSize;
            params.ch = m_config.m_cellHeight;
            params.buildBvTree = true;
            
            if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
            {
                NavSysLog("Could not build Detour navmesh.");
                return result;
            }
            
            result = std::shared_ptr<dtNavMesh>(dtAllocNavMesh(), DtDeleter{});
            if (!result)
            {
                dtFree(navData);
                NavSysLog("Could not create Detour navmesh");
                return result;
            }
            
            dtStatus status;
            
            status = result->init(navData, navDataSize, DT_TILE_FREE_DATA);
            if (dtStatusFailed(status))
            {
                dtFree(navData);
                NavSysLog("Could not init Detour navmesh");
                return result;
            }

            SaveNavMesh(land_navmesh_path, result.get());
        }

        #ifdef TIME_PROFILE
            LARGE_INTEGER endTime;
            QueryPerformanceCounter(&endTime);

            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            const auto diff = (endTime.QuadPart - startTime.QuadPart);
            NavSysLog("=== TOTAL:\t%.2fms", (diff*1000000 / freq.QuadPart) / 1000.0f);
        #endif

        return result;
    });
}