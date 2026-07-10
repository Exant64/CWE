
#include "stdafx.h"
#include "..//SA2ModLoader.h"
#include "..//Chao.h"
#include "../al_world.h"
#include "../ALifeSDK_Functions.h"
#include "../ninja_functions.h"
#include <random>
#include "al_knowledge.h"
#include <al_chao_info.h>

Bool AL_KW_IsSToyFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;

	if (kwlg->SToyFlag & (1 << Kind))
		return TRUE;

	return FALSE;
}

void AL_KW_SongFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;
	kwlg->SongFlag |= 1 << Kind;
}

Bool AL_KW_IsSongFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;

	if (kwlg->SongFlag & (1 << Kind))
		return TRUE;

	return FALSE;
}

void AL_KW_ArtFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;
	kwlg->ArtFlag |= 1 << Kind;
}

Bool AL_KW_IsArtFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;

	if (kwlg->ArtFlag & (1 << Kind))
		return TRUE;

	return FALSE;
}

void AL_KW_MusicFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;

	//cwe enum
	if (Kind >= NB_AL_MUSIC) {
		GET_CWEPARAM(pChaoTask)->MusicFlag_CWE |= 1 << (Kind - NB_AL_MUSIC);
		return;
	}
	
	kwlg->MusicFlag |= 1 << Kind;
}

Bool AL_KW_IsMusicFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge; 
		
	//cwe enum
	if (Kind >= NB_AL_MUSIC) {
		if (GET_CWEPARAM(pChaoTask)->MusicFlag_CWE & (1 << (Kind - NB_AL_MUSIC)))
			return TRUE;

		return FALSE;
	}

	if (kwlg->MusicFlag & (1 << Kind))                      
		return TRUE;                                           

	return FALSE;
}

void AL_KW_DanceFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;
	kwlg->DanceFlag |= 1 << Kind;
}

Bool AL_KW_IsDanceFlagOn(task* pChaoTask, int Kind) {
	AL_KNOWLEDGE_BTL* kwlg = &GET_CHAOPARAM(pChaoTask)->knowledge;
	
	switch (Kind) {
	case AL_DANCE_HERO:
		return AL_KW_IsDanceFlagOn(pChaoTask, AL_DANCE_FURIRFURI) && AL_KW_IsDanceFlagOn(pChaoTask, AL_DANCE_BOX);
	case AL_DANCE_DARK:
		return AL_KW_IsDanceFlagOn(pChaoTask, AL_DANCE_KURUKURU) && AL_KW_IsDanceFlagOn(pChaoTask, AL_DANCE_GOGO);
	case AL_DANCE_BREAK:
		//if any of the vanilla dances arent unlocked, return false
		//if it goes through the loop without returning then all of them are unlocked => true
		for (int i = 0; i < NB_AL_DANCE; i++) {
			if (!AL_KW_IsDanceFlagOn(pChaoTask, i)) {
				return FALSE;
			}
		}

		return TRUE;
	}

	if (kwlg->DanceFlag & (1 << Kind))
		return TRUE;

	return FALSE;
}

bool AL_KW_CompareID(const CHAO_ID& id1, const CHAO_ID& id2)
{
	return (id1.id[0] != 0 &&
		id2.id[0] == id1.id[0] &&
		id2.id[1] == id1.id[1] &&
		id2.gid[0] == id1.gid[0]);
}

int AL_KW_AlreadyKnows(task* a1, task* a2)
{
	chaowk* wk1 = GET_CHAOWK(a1);
	chaowk* wk2 = GET_CHAOWK(a2);

	for (int i = 0; i < 20; i++)
	{
		if (AL_KW_CompareID(wk2->pParamGC->ChaoID, wk1->pParamGC->knowledge.chao[i].id))
			return i;
	}
	return -1;
}

int AL_KW_GetRelationIndex(task* a1, task* a2)
{
	chaowk* wk1 = GET_CHAOWK(a1);
	chaowk* wk2 = GET_CHAOWK(a2);

	//if already registered, find it
	int alreadyKnowsID = AL_KW_AlreadyKnows(a1, a2);
	if (alreadyKnowsID > -1)
		return alreadyKnowsID;

	//if not registered, find new slot for it
	for (int i = 0; i < 20; i++)
	{
		if (wk1->pParamGC->knowledge.chao[i].id.id[0] == 0)
		{
			wk1->pParamGC->knowledge.chao[i].id = wk2->pParamGC->ChaoID;
			return i;
		}

	}

	//if nothing is available, invalid 
	return -1;
}

bool AL_KW_IDExists(const CHAO_ID& id)
{
	for (size_t i = 0; i < ChaoInfo::Instance().Count(); i++)
	{
		if (AL_KW_CompareID(ChaoInfo::Instance()[i].ChaoID, id))
			return true;
	}
	return false;
}

CHAO_PARAM_GC* AL_KW_FindChaoBasedOnId(const CHAO_ID& id)
{
	for (size_t i = 0; i < ChaoInfo::Instance().Count(); i++)
	{
		if (AL_KW_CompareID(ChaoInfo::Instance()[i].ChaoID, id))
			return &ChaoInfo::Instance()[i];
	}
	return nullptr;
}

int AL_KW_GetFriendCount(task* a1)
{
	int count = 0;
	chaowk* wk1 = GET_CHAOWK(a1);
	for (int i = 0; i < 20; i++)
	{
		if (wk1->pParamGC->knowledge.chao[i].id.id[0] != 0)
			count++;
	}
	return count;
}

void __cdecl AL_KW_MeetChao(task* a1, int index, KW_MEET_TYPE meetType)
{
	chaowk* wk1 = GET_CHAOWK(a1);

	//increment meet
	if (wk1->pParamGC->knowledge.chao[index].meet < 0xFFFFu)
	{
		wk1->pParamGC->knowledge.chao[index].meet++;
	}

	//likeness goes up by random
	//float val = njRandom();
	//if (val >= 0.5f)
	//wk1->pParamGC->Knowledge.chao[index].like++;
	//else if (val < 0.15f)
	//wk1->pParamGC->Knowledge.chao[index].like--;
		
	//limit likeness
	if (wk1->pParamGC->knowledge.chao[index].like > 50) wk1->pParamGC->knowledge.chao[index].like = 50;
	if (wk1->pParamGC->knowledge.chao[index].like < -50) wk1->pParamGC->knowledge.chao[index].like = -50;

	// wk1->pParamGC->knowledge.chao[index].meetType = meetType;

	wk1->pParamGC->knowledge.chao[index].distance = 2 * (wk1->pParamGC->knowledge.chao[index].distance / 3);
}

void AL_KW_AddLikeChao(task* a1, int index, int val)
{
	chaowk* wk1 = GET_CHAOWK(a1);
	wk1->pParamGC->knowledge.chao[index].like += val;
	if (wk1->pParamGC->knowledge.chao[index].like > 50) wk1->pParamGC->knowledge.chao[index].like = 50;
	if (wk1->pParamGC->knowledge.chao[index].like < -50) wk1->pParamGC->knowledge.chao[index].like = -50;
}

int AL_KW_GetMeetChao(task* a1, int index)
{
	if (index == -1)
	{
		//error
		return -1;
	}
	chaowk* wk1 = GET_CHAOWK(a1);
	return wk1->pParamGC->knowledge.chao[index].meet;
}

int AL_KW_GetMeetChao(task* a1, task* a2)
{
	return AL_KW_GetMeetChao(a1, AL_KW_GetRelationIndex(a1, a2));
}

void AL_KW_MeetEachother(task* a1, task* a2, KW_MEET_TYPE type)
{
	int relationIndex1 = AL_KW_GetRelationIndex(a1, a2);
	int relationIndex2 = AL_KW_GetRelationIndex(a2, a1);
	if (relationIndex1 > -1)
		AL_KW_MeetChao(a1, relationIndex1, type);
	if (relationIndex2 > -1)
		AL_KW_MeetChao(a2, relationIndex2, type);
}
