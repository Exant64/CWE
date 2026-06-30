#pragma once

#include <SA2ModLoader.h>
#include <cstdint>
#include <al_emotion.h>

enum GENE_FACE_CHECK {
	GENE_FACE_CHECK_LOW,
	GENE_FACE_CHECK_MID,
	GENE_FACE_CHECK_HIGH
};

struct GENE_FACE_DATA {
	uint8_t Eye;
	uint8_t Mouth;
	uint8_t PersonalityCount;
	uint8_t PersonalityKinds[3];
	uint8_t PersonalityCheckRanges[3];
};

// EM_PER_CALM = Carefree
// EM_PER_SLEEPY_HEAD = Naive
// "crybaby" is just low EM_PER_AGRESSIVE
static const GENE_FACE_DATA NewFaceEntries[] = {	
    // roax suggestions
	{ ChaoEyes_Normal, ChaoMouth_ClosedSmile2, 3, {EM_PER_CALM, EM_PER_SLEEPY_HEAD, EM_PER_CHARM}, {GENE_FACE_CHECK_MID, GENE_FACE_CHECK_MID, GENE_FACE_CHECK_MID}},
    { ChaoEyes_Mean, ChaoMouth_ClosedFrown2, 3, {EM_PER_SOLITUDE, EM_PER_FICKLE, EM_PER_KINDNESS}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_LOW}},
    { ChaoEyes_Circles, ChaoMouth_OpenSmile, 2, {EM_PER_VITALITY, EM_PER_AGRESSIVE}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
    { ChaoEyes_ClosedDown, ChaoMouth_OpenFrown, 2, {EM_PER_AGRESSIVE, EM_PER_FICKLE}, {GENE_FACE_CHECK_LOW, GENE_FACE_CHECK_LOW}},
    { ChaoEyes_ClosedDown, ChaoMouth_ToothyFrown, 2, {EM_PER_AGRESSIVE, EM_PER_FICKLE}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
    { ChaoEyes_ClosedUp, ChaoMouth_WideOpen, 2, {EM_PER_GLUTTON, EM_PER_VITALITY}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},

    // eru img column 1
	{ ChaoEyes_ClosedUp, ChaoMouth_ClosedSmile2, 3, {EM_PER_CALM, EM_PER_SLEEPY_HEAD, EM_PER_CHARM}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
	{ ChaoEyes_HalfClosed, ChaoMouth_ClosedSmile2, 3, {EM_PER_CALM, EM_PER_SLEEPY_HEAD, EM_PER_CHARM}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_LOW}},
	{ ChaoEyes_Mean, ChaoMouth_ClosedSmile2, 3, {EM_PER_CALM, EM_PER_SLEEPY_HEAD, EM_PER_CHARM}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_LOW, GENE_FACE_CHECK_LOW}},
	{ ChaoEyes_Mean, ChaoMouth_ClosedFrown, 3, {EM_PER_CALM, EM_PER_SLEEPY_HEAD, EM_PER_CHARM}, {GENE_FACE_CHECK_LOW, GENE_FACE_CHECK_LOW, GENE_FACE_CHECK_LOW}},

    // eru img column 2
    { ChaoEyes_HalfClosed, ChaoMouth_ClosedFrown, 2, {EM_PER_CALM, EM_PER_SKILLFUL}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
    { ChaoEyes_HalfClosed, ChaoMouth_ClosedFrown3, 2, {EM_PER_CALM, EM_PER_SKILLFUL}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_LOW}},
    { ChaoEyes_Tiny, ChaoMouth_ClosedFrown, 2, {EM_PER_CALM, EM_PER_SKILLFUL}, {GENE_FACE_CHECK_LOW, GENE_FACE_CHECK_LOW}},

    // eru img column 3
    { ChaoEyes_Tiny, ChaoMouth_OpenSmile, 2, {EM_PER_SLEEPY_HEAD, EM_PER_CHATTY}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
    { ChaoEyes_Tiny, ChaoMouth_ToothySmile, 2, {EM_PER_CALM, EM_PER_GLUTTON}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
    { ChaoEyes_Painful, ChaoMouth_ClosedFrown, 2, {EM_PER_FICKLE, EM_PER_CHARM}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_LOW}},
    { ChaoEyes_Painful, ChaoMouth_ClosedSmile, 2, {EM_PER_FICKLE, EM_PER_CHARM}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
    { ChaoEyes_Painful, ChaoMouth_ClosedFrown3, 2, {EM_PER_FICKLE, EM_PER_SLEEPY_HEAD}, {GENE_FACE_CHECK_HIGH, GENE_FACE_CHECK_HIGH}},
};