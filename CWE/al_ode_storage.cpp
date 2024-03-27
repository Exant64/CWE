#include "stdafx.h"
#include "SA2ModLoader.h"
#include "alg_kinder_he.h"
#include "al_odekake.h"
#include "ninja_functions.h"
#include "ui/selectable.h"
#include "ui/al_ortho.h"
#include "al_sandhole.h"
#include "memory.h"

#include "al_ode_menu.h"

#include <fstream>

void someUIProjectionCode(NJS_VECTOR* a1, NJS_VECTOR* a2);

//split this into another file
struct AL_ChaoSelect {
	ChaoData* m_chaoData;
	int m_chaoDataCount;

	std::vector<ObjectMaster*>* m_chaoTasks;

	float m_posX, m_posY; //top left corner
	int m_zoom;
	int m_nbX, m_nbY;
	int m_page;
};

void AL_ChaoSelect_DeleteChao(AL_ChaoSelect* chaoSelect) {
	auto& vec = *chaoSelect->m_chaoTasks;

	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i]) {
			DeleteObject_(vec[i]);
			vec[i] = 0;
		}
	}
}
static Light BM_MenuLight = { {  0.1f, -0.7f, -0.7f }, 1,  0.5f, {  1,  1,  1 } };

void ChaoDisplayHookTest(ObjectMaster* tp) {

	Lights[10] = BM_MenuLight;
	int backupLI = LightIndex;
	LightIndex = 10;

	//OrthoSetIndex(1);
	OrthoDrawBegin();

	njPushMatrixEx();
	OrthoScreenTranslate(tp->Data1.Entity->Position.x, tp->Data1.Entity->Position.y, 1);

	NJS_VECTOR backupPos = tp->Data1.Entity->Position;
	tp->Data1.Entity->Position = { 0,0,0 };
	Chao_Display(tp);
	tp->Data1.Entity->Position = backupPos;

	OrthoDrawEnd();
	njPopMatrixEx();

	LightIndex = backupLI;
}

void AL_ChaoSelect_UpdateList(AL_ChaoSelect* chaoSelect) {
	//call might look redundant, but i have a feeling it will be useful one day lol
	AL_ChaoSelect_DeleteChao(chaoSelect);

	auto& vec = *chaoSelect->m_chaoTasks;
	int chaoSizeX = 40 + chaoSelect->m_zoom * 10;
	int chaoSizeY = 50 + chaoSelect->m_zoom * 10;

	for (size_t i = 0; i < chaoSelect->m_chaoTasks->size(); i++) {
		NJS_VECTOR posIn, posOut;
		posIn.x = chaoSelect->m_posX + (i % chaoSelect->m_nbX) * (float)chaoSizeX;
		posIn.y = chaoSelect->m_posY + (i / chaoSelect->m_nbX) * (float)chaoSizeY;
		posIn.z = (float)(-54 + chaoSelect->m_zoom * 10); //-31
		someUIProjectionCode(&posIn, &posOut);

		if (chaoSelect->m_chaoTasks->at(i)) {
			PrintDebug("AL_ChaoSelect_UpdateList: the vector should be empty, returning");
			return;
		}

		ObjectMaster* obj = CreateChao(&chaoSelect->m_chaoData[i], 0, 0, &posIn, 0);
		obj->DisplaySub = ChaoDisplayHookTest;
		if (obj) {
			obj->Data1.Chao->field_B0 &= ~8u;
			obj->Data1.Chao->field_B0 &= ~2u;
			obj->Data1.Chao->field_B0 &= ~0x10u;
			chaoSelect->m_chaoTasks->at(i) = obj;
		}
		else {
			PrintDebug("AL_ChaoSelect_UpdateList: couldn't create chao at slot %d in %x", i, chaoSelect->m_chaoData);
		}
	}
}

//used incase we wanna switch the storage it points to
void AL_ChaoSelect_UpdateChaoData(AL_ChaoSelect* chaoSelect, ChaoData* pChaoData, int maxChao) {
	chaoSelect->m_chaoData = pChaoData;
	chaoSelect->m_chaoDataCount = maxChao;

	//we delete before so the resize doesnt cut off any existing chao
	AL_ChaoSelect_DeleteChao(chaoSelect);

	int nbPerPage = min(maxChao, chaoSelect->m_nbX * chaoSelect->m_nbY);
	chaoSelect->m_chaoTasks->resize(nbPerPage);

	AL_ChaoSelect_UpdateList(chaoSelect);
}

AL_ChaoSelect* Create_AL_ChaoSelect(float x, float y, ChaoData* pChaoData, int maxChao, int nbX, int nbY) {
	AL_ChaoSelect* chaoSelect = ALLOC(AL_ChaoSelect);

	chaoSelect->m_posX = x;
	chaoSelect->m_posY = y;

	chaoSelect->m_nbX = nbX;
	chaoSelect->m_nbY = nbY;

	chaoSelect->m_zoom = 1;

	chaoSelect->m_chaoTasks = new std::vector<ObjectMaster*>();

	AL_ChaoSelect_UpdateChaoData(chaoSelect, pChaoData, maxChao);

	return chaoSelect;
}

std::vector<ChaoData> LoadedFolderTest;

void AddChaoFile(const char* fileName) {
	struct ChaoFile {
		uint8_t header[64];
		ChaoData param;
	};

	auto buffer = std::make_unique<ChaoFile>();

	if (!buffer) {
		return;
	}

	char path[MAX_PATH];
	sprintf(path, "%s\\%s", "ChaoFiles", fileName);

	std::ifstream chaofile (path, std::ios::binary);
	if (!chaofile.good()) {
		return;
	}

	if (chaofile.read((char*)buffer.get(), sizeof(ChaoFile))) {
		LoadedFolderTest.push_back(buffer->param);
	}
}

void LoadChaoFiles() {
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA("ChaoFiles/*.chao", &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				AddChaoFile(fd.cFileName);
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
}

DataArray(ChaoHudThingB, MenuArray, 0x11BA528, 0x61);
void BackButtonDisp(ObjectMaster* a1) {

	float v14 = 320;
	float a3 = 470;
	float v9 = a3 - 5.0f;
	float v7 = (njSin(a1->Data1.Entity->Rotation.y) *  0.1f) + 1.0f;
	float sizeX = 40.0f;

	*(char*)0x25EFFCC = 1;
}

void CreateBackButton() {
	LoadObject(4, "back", [](ObjectMaster*) {}, LoadObj_Data1)->field_1C = BackButtonDisp;
}

void AL_OdekakeStorage(ODE_MENU_MASTER_WORK* a1)
{
	switch (a1->mode)
	{
	case 0:
		LargeTitleBarExecutor_Load(AL_OdekakeMenuMaster_Data_ptr->CurrStage, 650, 66);
		CreateBackButton();
		//LoadLuaObject("ui//storage.lua");
		//Create_AL_ChaoSelect(30, 130, ChaoSlots, 24, 4, 2);
		//LoadChaoFiles();
		//Create_AL_ChaoSelect(320, 130, LoadedFolderTest.data(), LoadedFolderTest.size(), 4, 6);
		a1->mode++;
		break;
	case 1:
		break;
	case 2:
		AL_OdeMenuSetNextStage(0);
		AL_OdeMenuChangeStage();
		break;
	}
}