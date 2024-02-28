#pragma once

enum {
	AL_LESSON_DANCE_FURIRFURI = 0x0,
	AL_LESSON_DANCE_KURUKURU = 0x1,
	AL_LESSON_DANCE_BOX = 0x2,
	AL_LESSON_DANCE_GOGO = 0x3,
	AL_LESSON_DANCE_RADIO = 0x4,

	AL_LESSON_MUSIC_BELL = 5,
	AL_LESSON_MUSIC_CASTANETS = 6,
	AL_LESSON_MUSIC_CYMBAL = 7,
	AL_LESSON_MUSIC_DRUM = 8,
	AL_LESSON_MUSIC_FUE = 9,
	AL_LESSON_MUSIC_MARACAS = 10,
	AL_LESSON_MUSIC_RAPPA = 11,
	AL_LESSON_MUSIC_TAMBOURINE = 12,
	AL_LESSON_MUSIC_ORGAN = 13,
	AL_LESSON_MUSIC_PIANO = 14,
	
	//15,16 would end up being the sing and art text, 19 is "today's lesson", 17 and 18 is unused "this week/month's lesson" text so we'll sacrifice that
	//and we'll sacrifice the organ for now since we only have 2 slots and we have 3 instruments
	AL_LESSON_MUSIC_GUITAR = AL_LESSON_MUSIC_ORGAN, //we'll sacrifice the organ for now
	AL_LESSON_MUSIC_TRIANGLE = 17,
	AL_LESSON_MUSIC_ACCORDION = 18,

	AL_LESSON_SING = 20,

	AL_LESSON_ART = 30
};

void alg_kinder_cl_Init();