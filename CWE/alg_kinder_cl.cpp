#include "stdafx.h"
#include "SA2ModLoader.h"
#include "al_msg_font.h"
#include "memory.h"
#include <stdio.h>
#include "ChaoMain.h"
#include "alg_kinder_cl.h"
#include <al_behavior/al_knowledge.h>
#include <asmutil.h>

enum eAL_SONG {
	AL_SONG_1 = 0x0,
	AL_SONG_2 = 0x1,
	AL_SONG_3 = 0x2,
	AL_SONG_4 = 0x3,
	AL_SONG_5 = 0x4,
	NB_AL_SONG = 0x5,
};

enum eAL_ART {
	AL_ART_1 = 0x0,
	AL_ART_2 = 0x1,
	AL_ART_3 = 0x2,
	AL_ART_4 = 0x3,
	AL_ART_5 = 0x4,
	NB_AL_ART = 0x5,
};

//info i gathered during research to hook the lesson ids
//msgID checks:
//LessonInProgressEntry
//Classroom_Main: 00586154
//something about already having learned the lesson: 00586714
//no idea: 00586D08
//i think this might be shifting the rest of the lessons up, filling in the empty spaces: 00586F2E

#pragma pack(push, 8)
struct ALIGN(2) KinWhole
{
	__int16 a;
	__int16 b;
};
#pragma pack(pop)

static ASM_FUNC const char* Classroom_GetMsg(char a1, int a2) {
    // arguments
    ASM_PUSH(      ASM_ESP(1+0+0) ); // a1
    ASM_MOVE( ecx, ASM_ESP(2+1+0) ); // a2

    // call
    ASM_CALL_R( edx, 0x00584B50 );

    // end arguments
    ASM_ESP_ADD( 1 );

    // return
    ASM_RET( 0 );
}

DataPointer(const char**, dword_1AED724, 0x1AED724);
DataPointer(KinWhole*, dword_1AED72C, 0x1AED72C);
DataPointer(const char**, msgKinWhole, 0x1AED6B4);
DataPointer(KinWhole*, kinWholeThing, 0x01AED728);
//we overwrite the strings we want with this
const char* Classroom_GetMsg_Custom(char a1, int a2) {
	const char** v2; // edx
	KinWhole* v3; // eax
	unsigned int v5; // ecx

	if (a1 == 1)
	{
		v2 = (const char**)dword_1AED724;
		v3 = (KinWhole*)dword_1AED72C;
	}
	else
	{
		v2 = (const char**)msgKinWhole;
		v3 = kinWholeThing;
	}
	if (!v3 || !v2)
	{
		return 0;
	}

	if (a2 == 99 + AL_LESSON_MUSIC_GUITAR)
		return "Guitar";

	if (a2 == 99 + AL_LESSON_MUSIC_TRIANGLE)
		return "Triangle";

	if (a2 == 99 + AL_LESSON_MUSIC_ACCORDION)
		return "Accordion";

	v5 = (unsigned int)v2[v3[a2].a];
	return (char*)v2 + ((((v5 << 16) | v5 & 0xFF00) << 8) | ((HIWORD(v5) | v5 & 0xFF0000) >> 8));
}

static void ASM_FUNC Classroom_GetMsg_hook() {
	ASM_PUSH(ecx); // a2
	ASM_PUSH(ASM_ESP(2)); // a1

	// Call your __cdecl function here:
	ASM_CALL (Classroom_GetMsg_Custom);

	ASM_ESP_ADD( 1 ); // a1
	ASM_POP(ecx); // a2
	ASM_RET(0);
}

//for the dance and music lessons it just checks if its already unlocked, those don't have any "levels"
//for singing and art, it checks all the flags, if any of them aren't it returns false and returns the level in pLessonLevel
//if none of the flags are on we assume all of them are learned so we return true
Bool IsLessonLearned(task* tp, int* pLessonLevel, int LessonKind) {
	CHAO_PARAM_GC* pParam = GET_CHAOPARAM(tp);

	*pLessonLevel = 0;

	//our custom ugly checks here
	switch (LessonKind) {
	case AL_LESSON_MUSIC_GUITAR:
		return AL_KW_IsMusicFlagOn(tp, AL_MUSIC_GUITAR);
	case AL_LESSON_MUSIC_TRIANGLE:
		return AL_KW_IsMusicFlagOn(tp, AL_MUSIC_TRIANGLE);
	case AL_LESSON_MUSIC_ACCORDION:
		return AL_KW_IsMusicFlagOn(tp, AL_MUSIC_ACCORDION);
	}

	if (LessonKind <= AL_LESSON_DANCE_RADIO) {
		return AL_KW_IsDanceFlagOn(tp, LessonKind);
	}

	if (LessonKind <= AL_LESSON_MUSIC_PIANO) {
		return AL_KW_IsMusicFlagOn(tp, LessonKind - 5);
	}

	if (LessonKind == AL_LESSON_SING) {
		for (int i = 0; i < NB_AL_SONG; i++) {
			if (!AL_KW_IsSongFlagOn(tp, i)) {
				*pLessonLevel = i;
				return FALSE;
			}
		}
		
		return TRUE;
	}

	if(LessonKind == AL_LESSON_ART) {
		for (int i = 0; i < NB_AL_ART; i++) {
			if (!AL_KW_IsArtFlagOn(tp, i)) {
				*pLessonLevel = i;
				return FALSE;
			}
		}
		
		return TRUE;
	}
	
	___OutputDebugString("IsLessonLearned: invalid lesson! %d", LessonKind);
	return TRUE;
}

static void ASM_FUNC IsLessonLearned_Hook() {
	ASM_PUSH(esi); // int a3
	ASM_PUSH(edi); // a2
	ASM_PUSH(edx); // a1

	// Call your __cdecl function here:
	ASM_CALL (IsLessonLearned);

	ASM_POP(edx); // a1
	ASM_POP(edi); // a2
	ASM_POP(esi); // int a3
	ASM_RET(0);
}

void SetLessonLearned(task* tp, char level, int LessonKind) {
	if (LessonKind == 30) {
		AL_KW_ArtFlagOn(tp, level);
	}
	else if (LessonKind == 20) {
		AL_KW_SongFlagOn(tp, level);
	}
	else if (LessonKind < 5) {
		if (LessonKind >= 0) {
			AL_KW_DanceFlagOn(tp, LessonKind);
		}
	}
	else {
		switch (LessonKind) {
		case AL_LESSON_MUSIC_GUITAR:
			AL_KW_MusicFlagOn(tp, AL_MUSIC_GUITAR);
			break;
		case AL_LESSON_MUSIC_TRIANGLE:
			AL_KW_MusicFlagOn(tp, AL_MUSIC_TRIANGLE);
			break;
		case AL_LESSON_MUSIC_ACCORDION:
			AL_KW_MusicFlagOn(tp, AL_MUSIC_ACCORDION);
			break;
		default:
			AL_KW_MusicFlagOn(tp, LessonKind - 5);
			break;
		}
		
	}
}

static void ASM_FUNC SetLessonLearned_Hook() {
	ASM_PUSH(esi); // int LessonKind
	ASM_PUSH(edi); // level
	ASM_PUSH(eax); // a1

	// Call your __cdecl function here:
	ASM_CALL (SetLessonLearned);

	ASM_POP(eax); // a1
	ASM_POP(edi); // level
	ASM_POP(esi); // int LessonKind
	ASM_RET(0);
}

extern "C" __declspec(dllexport) int LessonRerollTimer = 108000;
extern "C" __declspec(dllexport) int LessonFinishTimer = 36000;

void AL_KinderMessageTimerExec(task* tp) {
	char* ourMsg = (char*)tp->twp->ang.x;
	char* origMsg = (char*)tp->twp->ang.y;
	int globaltimer = *(int*)0x01DEC62C;

	int timerFrames;

	//if "current lesson" font use this formula
	if (!tp->twp->btimer) {
		timerFrames = LessonRerollTimer - (globaltimer % LessonRerollTimer);
	}
	else {
		int* LessonStartTime = (int*)0x19F6E54;
		timerFrames = LessonFinishTimer - (globaltimer - LessonStartTime[tp->twp->btimer - 1]);
	}

	if (timerFrames < 0)
		timerFrames = 0;

	int timerSeconds = timerFrames / 60;
	int timerMinutes = timerSeconds / 60;

	sprintf(ourMsg, "%s (%02d:%02d)", origMsg, timerMinutes, timerSeconds % 60);
}

void AL_KinderMessageTimerDisp(task* tp) {
	MessageFontThing messageBuffer;
	char* ourMsg = (char*)tp->twp->ang.x;
	NJS_POINT3* pPos = &tp->twp->pos;
	NJS_BGRA* pColor = (NJS_BGRA*)&tp->twp->ang.z;
	NJS_COLOR textColor;
	textColor.argb.b = pColor->a;
	textColor.argb.g = pColor->b;
	textColor.argb.r = pColor->g;
	textColor.argb.a = pColor->r;

	//msg color
	//tp->twp->Rotation.z;

	*(int*)0x01A267D0 = textColor.color;
	AlMsgFontCreateCStr(Language == 0, (int)ourMsg, (int)&messageBuffer, 999);

	AlMsgFontDrawRegionScale2(-1, (MessageFontThing*)&messageBuffer, pPos->x, pPos->y, pPos->z, 32, 0, tp->twp->scl.x, tp->twp->scl.x);

	AlMsgFontDelete((MessageFontThing*)&messageBuffer);
}

void AL_KinderMessageTimerDest(task* tp) {
	syFree((void*)tp->twp->ang.x, __FILE__, __LINE__);
}

task* __cdecl AL_KinderPMessageExec_Timer(task* a1, AL_KinderPMessage* a2) {	
	const char* pMsg = Classroom_GetMsg(0, a2->msgID);

	//HUGE NASTY HACK
	//we use the original object to get the proper centered version of the text
	//but since we also want it to be centered WITH the timer, we add a placeholder timer to the message
	char buffer[128];
	sprintf(buffer, "%s (44:44)", pMsg);
	a2->string = buffer;
	a2->flags |= 4;
	task* orig = AL_KinderPMessageExec_Load(a1, a2);

	task* pTask = CreateChildTask(IM_TWK, AL_KinderMessageTimerExec, a1);
	pTask->twp->scl.x = orig->twp->scl.x;
	pTask->twp->pos = orig->twp->pos;

	//check if this is a "chao in class" lesson text
	if (a1->exec == (task_exec)0x587FC0) {
		//lesson index + 1 (we use 0 for the "Today's lesson" text)
		pTask->twp->btimer = a1->twp->id + 1;
	}

	DestroyTask(orig);

	//15 extra characters have to be enough right 
	pTask->twp->ang.x = (int)syMalloc(strlen(pMsg) + 15, __FILE__, __LINE__);
	pTask->twp->ang.y = (int)pMsg;
	pTask->twp->ang.z = a2->color;
	pTask->disp_dely = AL_KinderMessageTimerDisp;
	pTask->dest = AL_KinderMessageTimerDest;

	return pTask;
}

static void ASM_FUNC AL_KinderPMessageExec_LoadTimer() {
	ASM_PUSH(ASM_ESP(1)); // a2
	ASM_PUSH(eax); // a1

	// Call your __cdecl function here:
	ASM_CALL (AL_KinderPMessageExec_Timer);

	ASM_ESP_ADD( 1 ); // a1<eax> is also used for return value
	ASM_ESP_ADD( 1 ); // a2
	ASM_RET(0);
}

extern "C" __declspec(dllexport) int LessonLookupTable_new[] =
{
	AL_LESSON_SING,
	AL_LESSON_DANCE_FURIRFURI,

	//custom entry here
	AL_LESSON_MUSIC_GUITAR,

	AL_LESSON_ART,
	AL_LESSON_MUSIC_BELL,
	AL_LESSON_MUSIC_DRUM,
	AL_LESSON_SING,

	//custom entry here
	AL_LESSON_MUSIC_TRIANGLE,

	AL_LESSON_DANCE_KURUKURU,
	AL_LESSON_MUSIC_TAMBOURINE,
	AL_LESSON_ART,
	AL_LESSON_DANCE_GOGO,
	AL_LESSON_MUSIC_RAPPA,
	AL_LESSON_SING,
	AL_LESSON_MUSIC_MARACAS,
	AL_LESSON_DANCE_BOX,

	//custom entry here
	AL_LESSON_MUSIC_ACCORDION,

	AL_LESSON_ART,
	AL_LESSON_MUSIC_CASTANETS,
	AL_LESSON_MUSIC_FUE,
	AL_LESSON_SING,
	AL_LESSON_DANCE_RADIO,
	AL_LESSON_MUSIC_CYMBAL,
	AL_LESSON_ART
};

DataArray(int, LessonTable_orig, 0x011D2DC8, 21);

#pragma pack(push, 8)
struct ALIGN(16) ClassroomData
{
	char byte0;
	char field_1;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char currentLesson;
	char field_7;
	char field_8;
	char field_9;
	char field_A;
	int field_C;
	int timer;
	char field_14;
};
#pragma pack(pop)

DataPointer(ClassroomData*, ClassroomDataPtr, 0x01AED73C);

static char LessonTableHack() {
	return LessonLookupTable_new[(ClassroomDataPtr->timer / LessonRerollTimer) % LengthOfArray(LessonLookupTable_new)];
}

void alg_kinder_cl_Init() {
	//nasty lessontable lookup hack, null out everything till table lookup instruction
	WriteData<52>((char*)0x005860F8, (char)0x90);
	WriteCall((void*)0x005860F8, (void*)LessonTableHack);

	//fixing "lesson already learned" text
	WriteData((char*)0x00586723, (char)19);

	WriteJump((void*)0x00585E00, (void*)IsLessonLearned_Hook);
	WriteJump((void*)0x00585EC0, (void*)SetLessonLearned_Hook);

	WriteJump((void*)0x00584B50, (void*)Classroom_GetMsg_hook);

	if (gConfigVal.ClassroomTimerDisplay) {
		WriteCall((void*)0x005861BF, (void*)AL_KinderPMessageExec_LoadTimer);
		WriteCall((void*)0x00588485, (void*)AL_KinderPMessageExec_LoadTimer);
	}
}