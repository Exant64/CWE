#include "stdafx.h"
#include <unordered_set>
#include <optional>
#include <unordered_map>
#include <deque>
#include <cnk_util.h>

// functions borrowed from Shaddatic's SAMT, thanks shad
size_t mtCnkVListSize(const Sint32* pVList) {
	const Sint32* start = pVList;
	const Sint32* vlist = pVList;

	while (CNK_GET_OFFTYPE(vlist) != 0xFF)
	{
		/** Next data chunk **/
		vlist += ((uint16_t*)vlist)[1] + 1;
	}

	return ((uintptr_t)vlist - (uintptr_t)start) + 4;
}

size_t mtCnkPListSize(const Sint16* pPList) {
	const Sint16* start = pPList;
	const Sint16* plist = pPList;

	for (; ; )
	{
		const int type = CNK_GET_OFFTYPE(plist);

		if (type == NJD_CE)
		{
			/** NJD_ENDOFF **/
			break;
		}

		if (type == NJD_CN)
		{
			/** NJD_NULLOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type <= CNK_BITSOFF_MAX)
		{
			/** NJD_BITSOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type <= CNK_TINYOFF_MAX)
		{
			/** NJD_TINYOFF **/

			/** Next offset **/
			plist += 2;
			continue;
		}

		if (type <= CNK_MATOFF_MAX)
		{
			/** NJD_MATOFF **/

			/** Next offset **/
			plist += ((uint16_t*)plist)[1] + 2;
			continue;
		}

		if (type <= CNK_STRIPOFF_MAX) // and volume
		{
			/** NJD_STRIPOFF **/

			/** Next offset **/
			plist += ((uint16_t*)plist)[1] + 2;
			continue;
		}

		/** An error occured, stop **/
		break;
	}

	return ((uintptr_t)plist - (uintptr_t)start) + 2;
}


// this whole adjacency code is by far one of the worst pieces of code i produced in recent years
// i'm sorry if you have to read this

struct ChunkFace {
	size_t polyChunkIndex;
	size_t stripIndex;
	size_t faceIndex;

	bool operator==(const ChunkFace& other) const
	{
		return (polyChunkIndex == other.polyChunkIndex
			&& stripIndex == other.stripIndex
			&& faceIndex == other.faceIndex);
	}

	bool operator<(const ChunkFace& other) const
	{
		return (polyChunkIndex < other.polyChunkIndex
			&& stripIndex < other.stripIndex
			&& faceIndex < other.faceIndex);
	}
};

template <>
struct std::hash<ChunkFace>
{
	std::size_t operator()(const ChunkFace& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return std::hash<size_t>()(k.polyChunkIndex * 32768 * 4 + k.stripIndex * 32768 + k.faceIndex);
	}
};

struct FaceVert {
	size_t v1;
	size_t v2;

	bool operator==(const FaceVert& other) const
	{
		return (v1 == other.v1
			&& v2 == other.v2);
	}

	bool operator<(const FaceVert& other) const
	{
		return (v1 < other.v1
			&& v2 < other.v2);
	}
};

template <>
struct std::hash<FaceVert>
{
	std::size_t operator()(const FaceVert& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return ((std::hash<size_t>()(k.v1)
			^ (std::hash<size_t>()(k.v2) << 1)) >> 1);
	}
};


struct IndexCacheEntry {
	size_t polyChunkIndex;
	size_t stripIndex;

	bool operator<(const IndexCacheEntry& other) const
	{
		return (polyChunkIndex < other.polyChunkIndex
			&& stripIndex < other.stripIndex);
	}

	bool operator==(const IndexCacheEntry& other) const
	{
		return (polyChunkIndex == other.polyChunkIndex
			&& stripIndex == other.stripIndex);
	}
};

template <>
struct std::hash<IndexCacheEntry>
{
	std::size_t operator()(const IndexCacheEntry& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return ((std::hash<size_t>()(k.polyChunkIndex)
			^ (std::hash<size_t>()(k.stripIndex) << 1)) >> 1);
	}
};


using IndexCache = std::unordered_map<IndexCacheEntry, std::vector<Uint16>>;
// christ
static std::unordered_map< ChunkFace, std::vector< ChunkFace>> GetAdjacency(
	const NJS_CNK_MODEL* pModel, IndexCache& indicesCache)
{
	std::unordered_map< FaceVert, std::vector< ChunkFace>> map;
	std::unordered_map< ChunkFace, std::vector< ChunkFace>> adjacency;

	const Sint16* start = pModel->plist;
	const Sint16* plist = pModel->plist;
	size_t plistIndex = 0;

	for (; ; )
	{
		const int type = CNK_GET_OFFTYPE(plist);
		const size_t currPlistIndex = plistIndex++;

		if (type == NJD_CE)
		{
			/** NJD_ENDOFF **/
			break;
		}

		if (type == NJD_CN)
		{
			/** NJD_NULLOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type <= CNK_BITSOFF_MAX)
		{
			/** NJD_BITSOFF **/

			/** Next offset **/
			++plist;
			continue;
		}

		if (type <= CNK_TINYOFF_MAX)
		{
			/** NJD_TINYOFF **/

			/** Next offset **/
			plist += 2;
			continue;
		}

		if (type <= CNK_MATOFF_MAX)
		{
			/** NJD_MATOFF **/

			/** Next offset **/
			plist += ((uint16_t*)plist)[1] + 2;
			continue;
		}

		if (type <= CNK_STRIPOFF_MAX) // and volume
		{
			/** NJD_STRIPOFF **/
			size_t stripCount = ((uint16_t*)plist)[2] & 0x3FFF;
			Sint16 UserOffset = (plist[2] >> 14) & 3;
			const uint16_t* pStrip = (uint16_t*)plist + 3;

			for (size_t i = 0; i < stripCount; ++i) {
				const int indexCount = abs(*(Sint16*)pStrip);

				pStrip++;

				const IndexCacheEntry indexCacheKey = {
					currPlistIndex,
					i
				};

				auto& indicesVec = indicesCache[indexCacheKey];
				indicesVec = {};

				for (size_t j = 0; j < indexCount; ++j) {
					indicesVec.push_back(*pStrip);
					pStrip++;

					// skip uvs
					switch (type) {
					case NJD_CS_UVH:
					case NJD_CS_UVN:
					case NJD_CS_UVN_VN:
					case NJD_CS_UVH_VN:
					case NJD_CS_UVN_D8:
					case NJD_CS_UVH_D8:
						pStrip += 2;
						break;
					}

					// skip colors
					switch (type) {
					case NJD_CS_D8:
					case NJD_CS_UVN_D8:
					case NJD_CS_UVH_D8:
						pStrip += 2;
						break;
					}

					if (j >= 2) {
						pStrip += UserOffset;
					}
					// todo: handle skipping uv2 and userflags, can't find reference code for VN
				}

				for (size_t j = 0; j < indicesVec.size() - 2; ++j) {
					const size_t face0 = indicesVec[j];
					const size_t face1 = indicesVec[j + 1];
					const size_t face2 = indicesVec[j + 2];

					const auto check_set_map = [&](const size_t v1, const size_t v2) {
						const FaceVert faceVert = {
							.v1 = v1,
							.v2 = v2
						};

						const FaceVert faceVertSwapped = {
							.v1 = v2,
							.v2 = v1
						};

						const ChunkFace chunkFace = {
							.polyChunkIndex = currPlistIndex,
							.stripIndex = i,
							.faceIndex = j
						};

						if (map.contains(faceVert)) {
							map[faceVert].push_back(chunkFace);
						}
						else if (map.contains(faceVertSwapped)) {
							map[faceVertSwapped].push_back(chunkFace);
						}
						else {
							map[faceVert] = { };
							map[faceVert].push_back(chunkFace);
						}
						};

					check_set_map(face0, face1);
					check_set_map(face1, face2);
					check_set_map(face2, face0);
				}
			}

			/** Next offset **/
			plist += ((uint16_t*)plist)[1] + 2;
			continue;
		}

		/** An error occured, stop **/
		break;
	}

	for (const auto& pair : map) {
		if (pair.second.size() != 2) continue;

		if (!adjacency.contains(pair.second[0])) {
			adjacency[pair.second[0]] = {};
		}

		adjacency[pair.second[0]].push_back(pair.second[1]);

		if (!adjacency.contains(pair.second[1])) {
			adjacency[pair.second[1]] = {};
		}

		adjacency[pair.second[1]].push_back(pair.second[0]);
	}

	return adjacency;
}

std::optional<std::vector<Uint16>> GetNonAdjacentIndices(const NJS_CNK_MODEL* pModel, const size_t vertexIndex) {
	IndexCache indicesCache;
	const auto adjacency = GetAdjacency(pModel, indicesCache);

	std::vector<ChunkFace> faces;
	for (const auto& pair : adjacency) {
		faces.push_back(pair.first);
	}

	// create components
	std::vector<std::unordered_set<IndexCacheEntry>> components;
	std::unordered_set<ChunkFace> visited;
	for (const auto& face : faces) {
		if (visited.contains(face)) {
			continue;
		}

		std::deque<ChunkFace> queue;
		queue.push_back(face);

		std::unordered_set<IndexCacheEntry> component;
		while (!queue.empty()) {
			const auto f = queue.front();
			queue.pop_front();

			if (visited.contains(f)) {
				continue;
			}

			visited.insert(f);
			const IndexCacheEntry entry = {
				.polyChunkIndex = f.polyChunkIndex,
				.stripIndex = f.stripIndex
			};
			component.insert(entry);

			for (const auto& extend : adjacency.at(f)) {
				queue.push_front(extend);
			}
		}

		components.push_back(component);
	}

	// if there's only one component then there's nothing much to look for
	// there's no disjoint part of the mesh
	if (components.size() < 2) {
		return std::nullopt;
	}

	// scans through the found components
	// if any of the component's indices contains the vertex index, skip storing the indices
	// final indices stores the result of one or more components' indices
	std::vector<Uint16> finalIndices;
	for (const auto& component : components) {
		bool containsIndex = false;
		std::vector<Uint16> componentIndices;

		for (const auto& componentFace : component) {
			const auto& indices = indicesCache[componentFace];

			if (std::find(indices.begin(), indices.end(), Uint16(vertexIndex)) != indices.end()) {
				containsIndex = true;
				break;
			}

			componentIndices.insert(componentIndices.end(), indices.begin(), indices.end());
		}

		if (!containsIndex) {
			finalIndices.insert(finalIndices.end(), componentIndices.begin(), componentIndices.end());
		}
	}

	if (!finalIndices.size())
		return std::nullopt;

	return finalIndices;
}