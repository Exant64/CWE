#pragma once
#include "Chao.h"

enum eAL_MUSIC
{
	AL_MUSIC_BELL = 0x0,
	AL_MUSIC_CASTANETS = 0x1,
	AL_MUSIC_CYMBAL = 0x2,
	AL_MUSIC_DRUM = 0x3,
	AL_MUSIC_FUE = 0x4,
	AL_MUSIC_MARACAS = 0x5,
	AL_MUSIC_RAPPA = 0x6,
	AL_MUSIC_TAMBOURINE = 0x7,
	NB_AL_MUSIC,
};

enum eAL_MUSIC_CWE {
	AL_MUSIC_GUITAR = NB_AL_MUSIC,
	AL_MUSIC_TRIANGLE,
	AL_MUSIC_ACCORDION,
	NB_AL_MUSIC_CWE
};

enum eAL_DANCE
{
	AL_DANCE_FURIRFURI = 0x0,
	AL_DANCE_KURUKURU = 0x1,
	AL_DANCE_BOX = 0x2,
	AL_DANCE_GOGO = 0x3,
	AL_DANCE_RADIO = 0x4,
	NB_AL_DANCE = 0x5,
};

enum eAL_DANCE_CWE {
	AL_DANCE_HERO = NB_AL_DANCE,
	AL_DANCE_DARK,
	AL_DANCE_BREAK,
	NB_AL_DANCE_CWE
};

enum KW_MEET_TYPE : char
{
	KW_MEET_CHAT,
	KW_MEET_BAND,
	KW_MEET_EVOLVE,
	KW_MEET_REBIRTH
};
int AL_KW_AlreadyKnows(ObjectMaster* a1, ObjectMaster* a2);
void AL_KW_AddLikeChao(ObjectMaster* a1, int index, int val);
int AL_KW_GetFriendCount(ObjectMaster* a1);
void AL_KW_MeetEachother(ObjectMaster* a1, ObjectMaster* a2, KW_MEET_TYPE type);
int AL_KW_GetRelationIndex(ObjectMaster* a1, ObjectMaster* a2);
int AL_KW_GetMeetChao(ObjectMaster* a1, int index);
int AL_KW_GetMeetChao(ObjectMaster* a1, ObjectMaster* a2);
bool AL_KW_IDExists(const CHAO_ID& id);
ChaoDataBase* AL_KW_FindChaoBasedOnId(const CHAO_ID& id);

Bool AL_KW_IsSToyFlagOn(task* pChaoTask, int Kind);
Bool AL_KW_IsSongFlagOn(task* pChaoTask, int Kind);
Bool AL_KW_IsArtFlagOn(task* pChaoTask, int Kind);
Bool AL_KW_IsMusicFlagOn(task* pChaoTask, int Kind);
Bool AL_KW_IsDanceFlagOn(task* pChaoTask, int Kind);

void AL_KW_SongFlagOn(task* pChaoTask, int Kind);
void AL_KW_ArtFlagOn(task* pChaoTask, int Kind);
void AL_KW_MusicFlagOn(task* pChaoTask, int Kind);
void AL_KW_DanceFlagOn(task* pChaoTask, int Kind);