#pragma once

#include <stdint.h>
#include "ninja.h"
#include "SA2Enums.h"
#include <cstddef>

// SA2 Structs

typedef NJS_VECTOR Vector3;

struct taskwk;
struct ObjectListEntry;
struct COL;
struct ModelIndex;
struct SETEntry;
struct SETObjectData;
struct colliwk;
struct struct_0;
struct AnimationIndex;
struct AnimationInfo;
struct CollisionData;
struct CharObj2Base;
struct EntityData2;
struct LaunchConfig_vtable;
struct CHAO_PARAM_GC;
struct ChaoUnknownE;
struct ChaoDebugData1;
struct UnknownData2;
struct chaowk;
struct ObjUnknownA;
struct ObjUnknownB;
struct task;

typedef void(__cdecl *task_exec)(task *);

// All structs should be packed.
#pragma pack(push, 1)

struct AllocatedMem
{
	int Cookie;
	void *Data;
};

union __declspec(align(2)) Data1Ptr
{
	void *Undefined;
	taskwk *Entity;
	ChaoDebugData1 *ChaoDebug;
};

union Data2Ptr
{
	void *Undefined;
	ObjUnknownB *UnknownB;
	EntityData2 *Entity;
	CharObj2Base *Character;
	UnknownData2 *Unknown_Chao;
};

struct task
{
	task *PrevObject;
	task *NextObject;
	task *Parent;
	task *Child;
	task_exec exec;
	task_exec disp;
	task_exec dest;
	task_exec field_1C;
	task_exec field_20;
	task_exec SomethingSub;
	task_exec field_28;
	void *field_2C;
	SETObjectData *SETData;
	taskwk* twp;
	UnknownData2 *EntityData2;
	ObjUnknownA *UnknownA_ptr;
	Data2Ptr Data2;
	char *Name;
	char *NameAgain;
	void *field_4C;
};

struct SETObjectData
{
	uint8_t LoadCount;
	char field_1;
	__int16 Flags;
	task *Object;
	SETEntry *SETEntry;
	float field_C;
};

struct Angle3
{
	int x;
	int y;
	int z;
};

struct taskwk
{
	char mode;
	char smode;
	char id;
	char btimer;
	__int16 flag;
	__int16 wtimer;
	Angle3 ang;
	NJS_POINT3 pos;
	NJS_VECTOR scl;
	colliwk *cwp;
};

struct CNK_VN_VERTEX {
	NJS_POINT3 Pos;
	NJS_POINT3 Normal;
};

struct al_model {
	int* VList;
	__int16* PList;
	NJS_POINT3 Center;
	float Radius;
	__int16 OrgTexId[4];
	int PListSize;
	int nbVertex;
	CNK_VN_VERTEX* pVertex;
};

typedef struct {
    float Spring1;
    float Spring2;
    float Friction1;
    float Friction2;
    float MaxDiff;
    float MaxSpd;
    float Weight1;
    float Weight2;
    float Weight3;
    NJS_POINT3 Center;
} AL_BUYO_PARAM;

struct AL_OBJECT
{
	Uint32 EvalFlags;
	al_model *pModel;
	Float Pos[3];
	Angle Ang[3];
	Float Scl[3];
	struct AL_OBJECT *pChild;
	struct AL_OBJECT *pSibling;
	struct AL_OBJECT* pParent;
	NJS_POINT3 diff;
	NJS_POINT3 GlobalAimPos;
	NJS_POINT3 OrgPos;
	NJS_VECTOR Velo;
	NJS_POINT3 OrgAng;
    NJS_POINT3 AngSpd;
	float Weight;
    float Spring;
    float Friction;
    int32_t NoBuyoFlag;
    int32_t CalcBuyoPosFlag;
	NJS_OBJECT *pPartsObject;
	NJS_OBJECT* pItemObject;
	NJS_TEXLIST* pItemTexlist;
    float ItemScale;
    int32_t ItemActiveFlag;
	int ItemOffsetFlag;
	NJS_POINT3 ItemOffsetPos;
	Angle3 ItemOffsetAng;
	NJS_MATRIX_PTR pOldMatrix;
	AL_BUYO_PARAM* pBuyoParam;
    void* DisplayList;
};

#pragma pack(push, 8)
struct __declspec(align(4)) AL_FACE_CTRL
{
	int EyeTimer;
	__int16 EyeColorNum;
	__int16 EyeCurrNum;
	__int16 EyeDefaultNum;
	int MouthTimer;
	__int16 MouthCurrNum;
	__int16 MouthDefaultNum;
	float EyePosX;
	float EyePosY;
	float EyeSclX;
	float EyeSclY;
	unsigned int Flag;
	NJS_OBJECT* pEyeObject[2];
	NJS_OBJECT* pMouthObject;
	int EyeLidBlinkMode;
	int EyeLidBlinkTimer;
	int EyeLidBlinkAng;
	int EyeLidExpressionMode;
	int EyeLidExpressionTimer;
	int EyeLidExpressionDefaultCloseAng;
	int EyeLidExpressionCurrCloseAng;
	int EyeLidExpressionAimCloseAng;
	int EyeLidExpressionDefaultSlopeAng;
	int EyeLidExpressionCurrSlopeAng;
	int EyeLidExpressionAimSlopeAng;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) MotionTableAction
{
	NJS_MOTION *NJS_MOTION;
	__int16 FlagThing1;
	__int16 field_6;
	int TransitionToID;
	int field_C;
	float StartFrame;
	float EndFrame;
	float PlaySpeed;
};
#pragma pack(pop)

struct MOTION_TABLE
{
	NJS_MOTION* pMotion;
	short mode;
	short posture;
	int next;
	int link_step;
	float start;
	float end;
	float spd;
};

#pragma pack(push, 4)
typedef struct {
    uint16_t    mode;
    float         frame;
    float         start;
    float         end;
    float         spd;
    NJS_MOTION* pMotion;
} MOTION_INFO;
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) MOTION_CTRL
{
	short flag;
	__int16 posture;
	int curr_num;
	int next_num;
	float multi_spd;
	float link_spd;
	float ratio;
	MOTION_INFO minfo[2];
	MOTION_TABLE *table;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) AL_BEHAVIOR
{
	unsigned __int16 Flag;
	unsigned __int16 Mode;
	unsigned __int16 SubMode;
	unsigned __int16 MoveMode;
	int InterruptFlag;
	int Timer;
	int SubTimer;
	int LimitTimer;
	int BehaviorTimer;
	unsigned __int16 Intention;
	unsigned __int16 IntentionMode;
	unsigned __int16 IntentionSubMode;
	__int16 field_22;
	unsigned int IntentionTimer[17]; //18 in sadx
	unsigned int IntervalTimer[128];
	int FreeWork;
	float MoveRadius;
	NJS_POINT3 BasePos;
	int(__cdecl* PrevFunc)(task*);
	int nbBhvFuncEntry;
	int CurrBhvFuncNum;
	int(__cdecl* BhvFuncList[16])(task*);
	int ReserveTimerList[16];
};
#pragma pack(pop)

struct al_entry_work
{
	unsigned __int16 category;
	unsigned __int16 num;
	unsigned __int16 kind;
	unsigned __int16 flag;
	void* pSaveInfo;
	int CommuID;
	NJS_POINT3 pos;
	Angle3 ang;
	float radius;
	float offset;
	float CamDist;
	__int16 command;
	__int16 command_value;
	int state;
	task* tp;
	al_entry_work* pCommu;
	al_entry_work* pLockOn;
};


struct ChaoSomeUnknownA
{
	__int16 index;
	__int16 index2;
	__int16 ID;
	__int16 someFlag;
	int saveData;
	int field_C;
	NJS_VECTOR locationVector;
	int field_1C;
	int field_20;
	int field_24;
	float field_28;
	float field_2C;
	float playerDistance;
	__int16 setSomeIndex;
	__int16 field_36;
	__int16 field_38;
	__int16 field_3A;
	task* pointerToOwner;
	ChaoSomeUnknownA* heldBy;
	ChaoSomeUnknownA* field_44;
};

#pragma pack(push, 8)
struct __declspec(align(4)) al_perception_link
{
	__int16 info[4];
	float dist;
	int InSightFlag;
	int HearFlag;
	int SmellFlag;
	al_entry_work* pEntry;
};
#pragma pack(pop)

#pragma pack(push, 8)
struct __declspec(align(4)) AL_PERCEPTION_INFO
{
	int nbPerception;
	int InSightFlag;
	int HeardFlag;
	int SmellFlag;
	float NearestDist;
	int NearestNum;
	al_perception_link field_18[32];
};
#pragma pack(pop)

typedef struct AL_PERCEPTION {
    float SightRange;
    int SightAngle;
    int SightAngleHalf;
    float HearRange;
    float SmellRange;
    AL_PERCEPTION_INFO Player;
    AL_PERCEPTION_INFO Chao;
    AL_PERCEPTION_INFO Fruit;
    AL_PERCEPTION_INFO Tree;
    AL_PERCEPTION_INFO Toy;
    AL_PERCEPTION_INFO Sound;
} AL_PERCEPTION;

#pragma pack(push, 4)
typedef struct {
    uint16_t  Flag;
    uint16_t  CurrNum;
    float       Ratio;
    NJS_LINE  Plane;
} AL_ZONE;

typedef struct {
    uint16_t Mode;
    uint16_t TexNum;
    uint16_t Timer;
    NJS_POINT3 Offset;
    NJS_POINT3 Pos;
    NJS_POINT3 Velo;
    NJS_POINT3 Scl;
    NJS_POINT3 SclSpd;
} AL_ICON_INFO;

typedef struct {
    int16_t CurrType;
    int16_t NextType;
    int32_t Timer;
    int32_t NextTimer;
    int32_t PuniPhase;
    int32_t PosPhase;
    uint32_t Color;
    uint16_t TexAnimNum;
    uint16_t TexAnimTimer;
    int32_t ang;
    NJS_POINT3 Up;
    NJS_POINT3 Pos;
    AL_ICON_INFO Upper;
    AL_ICON_INFO Lower;
} AL_ICON;
#pragma pack(pop)

typedef struct al_group_object_list {
    NJS_OBJECT* child[40];
    NJS_OBJECT* normal[40];
    NJS_OBJECT* swim[40];
    NJS_OBJECT* fly[40];
    NJS_OBJECT* run[40];
    NJS_OBJECT* power[40];
} AL_GROUP_OBJECT_LIST;

typedef struct AL_SHAPE {
	AL_OBJECT* pObject;
	AL_OBJECT* CurrObjectList[40];

	AL_GROUP_OBJECT_LIST* pObjectList;
    AL_GROUP_OBJECT_LIST* pObjectListH;
    AL_GROUP_OBJECT_LIST* pObjectListD;

	NJS_POINT3 BodyPos;
	NJS_POINT3 HeadPos;
	NJS_POINT3 LeftHandPos;
	NJS_POINT3 RightHandPos;
	NJS_POINT3 LeftFootPos;
    NJS_POINT3 RightFootPos;
    NJS_POINT3 MouthPos;
    NJS_VECTOR MouthVec;
    NJS_VECTOR LeftEyePos;
    NJS_VECTOR LeftEyeVec;
    NJS_VECTOR RightEyePos;
    NJS_VECTOR RightEyeVec;

	NJS_OBJECT* pLeftHandItemObject;
	NJS_TEXLIST* pLeftHandItemTexlist;
	float LeftHandItemScale;
	float LeftHandItemActiveFlag;

	NJS_OBJECT* pRightHandItemObject;
	NJS_TEXLIST* pRightHandItemTexlist;
	float RightHandItemScale;
	float RightHandItemActiveFlag;

	unsigned __int16* palette;
	unsigned __int16 Flag;
	__int16 ColorNum;
	__int16 EnvNum;
	unsigned int IconColor;
	float SclH;
	float SclV;
	float CamDist;
} AL_SHAPE;

typedef struct {
    uint8_t  Exp[8];
    uint8_t  Abl[8];
    uint8_t  Lev[8];
    uint16_t Skills[8];
} TMP_PARAM;

#pragma pack(push, 8)
struct __declspec(align(4)) chaowk
{
	char mode;
	char smode;
	char id;
	char btimer;
	__int16 flag;
	__int16 wtimer;
	Angle3 ang;
	NJS_POINT3 pos;
	NJS_VECTOR scl;
	colliwk *cwp;

	int Timer;
	task* pMayu;
	task* pBooktask;
	Bool NestFlag;
	task* pAnytask;
	task *pAimtask;
	int32_t AimNum;
    int32_t RememberNum;
    int32_t pitch;
	float ClimbFirstPos;
	int field_58;
	CHAO_PARAM_GC *pParamGC;
	TMP_PARAM tmpParam;
	int Stamina;
	int AimStamina;
	task* tp;
    Angle pre_ang[3];
	int ChaoFlag;
	Uint16 ColliFormat;
	float CurrZone;
	MOTION_CTRL MotionCtrl;
	MOTION_CTRL MiniMotionCtrl;
	MOTION_TABLE MiniMotionTable[4];
	AL_BEHAVIOR Behavior;
	int field_4BC[21];
	AL_SHAPE Shape;
	AL_FACE_CTRL Face;
	AL_ICON Icon;
	AL_ZONE Zone;
	AL_PERCEPTION Perception;
	void* pWork;

	size_t LocalCharacterChaoType;
	bool IsCustomChaoTypeLoaded;
	unsigned char AnimRandomized;
	unsigned char ExtraSound;

	char AccessoryCalculatedID[4][21];
	Uint32 AccessoryIndices[4];

	bool JiggleFlagChanged;
	bool BaldHideHead;
	Uint16* pBaldAdjacencyIndices;
	size_t BaldAdjacencyIndexCount;

	void* BhvUserData[16];
};
#pragma pack(pop)

struct UnknownData2_B
{
	int field_0;
	char gap_4[12];
	float field_10;
	NJS_VECTOR field_14;
};

struct UnknownData2
{
	NJS_VECTOR velocity;
	NJS_VECTOR speed;
	char gap18[4];
	int field_1C;
	char gap20[8];
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	char gap38[4];
	int field_3C;
	__int16 field_40;
	__int16 Index;
	char gap44[4];
	float gravity;
	int gap_4C;
	int field_50;
	NJS_VECTOR Waypoint;
	NJS_VECTOR some_vector;
	char gap6C[4];
	float field_70;
	char gap74[28];
	float field_90;
	float field_94;
	float field_98;
	float field_9C;
	float field_A0;
	float field_A4;
	float field_A8;
	float field_AC;
	char gapB0[4];
	float field_B4;
	float field_B8;
	float field_BC;
	float field_C0;
	float field_C4;
	float field_C8;
	float field_CC;
	float field_D0;
	char gapD4[4];
	float field_D8;
	float field_DC;
	NJS_VECTOR field_E0;
	UnknownData2_B WeirdStructures[12];
};

struct ObjUnknownA
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	float field_24;
	float field_28;
	float field_2C;
	int field_30;
	int field_34;
};

struct ObjUnknownB
{
	int Time;
	int Index;
	int Mode;
	int field_C;
};

struct EntityData2
{
	CharObj2Base *CharacterData;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int field_30;
	float field_34;
	int field_38;
	float field_3C;
};

struct PhysicsData
{
	int HangTime;
	float anonymous_0;
	float HSpeedCap;
	float VSpeedCap;
	float BaseSpeed;
	float anonymous_4;
	float InitialJumpSpeed;
	float anonymous_6;
	float anonymous_7;
	float anonymous_8;
	float MinSpeedForSomethingButAlsoKnockbackSpeed;
	float anonymous_10;
	float anonymous_11;
	float anonymous_12;
	float JumpHoldSpeed;
	float GroundAccel;
	float AirAccel;
	float GroundDecel;
	float BrakeSpeed;
	float AirBrakeSpeed;
	float AirDecel;
	float anonymous_20;
	float anonymous_21;
	float anonymous_22;
	float anonymous_23;
	float anonymous_24;
	float anonymous_25;
	float anonymous_26;
	float anonymous_27;
	float CollisionSize;
	float Gravity;
	float CameraYOff;
	float YOff;
};

struct CharAnimInfo
{
	__int16 field_0;
	__int16 Animation;
	__int16 Animation2;
	__int16 Animation3;
	__int16 field_8;
	__int16 field_A;
	__int16 field_C;
	char field_E;
	char field_F;
	float field_10;
	float field_14;
	float field_18;
	char field_1C;
	char field_1D;
	char field_1E;
	char field_1F;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	AnimationInfo *Animations;
	NJS_MOTION *motion;
};

struct CharObj2Base
{
	char PlayerNum;
	char CharID;
	char Costume;
	char CharID2;
	char ActionWindowItems[8];
	char ActionWindowItemCount;
	char field_D[3];
	__int16 Powerups;
	int field_12;
	__int16 UnderwaterTime;
	__int16 IdleTime;
	char gap_1A[10];
	int Upgrades;
	float field_28;
	char field_2C[28];
	float MechHP;
	int field_4C[6];
	NJS_VECTOR Speed;
	char gap_70[36];
	task *HeldObject;
	char gap_98[4];
	task *HoldTarget;
	char gap_A0[28];
	NJS_MOTION **Animation;
	PhysicsData PhysData;
	int field_144[12];
	CharAnimInfo AnimInfo;
	int field_1A0[7];
};

struct SETEntry
{
	__int16 ID;
	__int16 XRot;
	__int16 YRot;
	__int16 ZRot;
	NJS_VECTOR Position;
	NJS_VECTOR Scale;
};

struct colliwk
{
	__int16 char0;
	__int16 field_2;
	uint16_t flag;
	uint16_t nbInfo;
	float field_8;
	CollisionData *CollisionArray;
	uint8_t f10[140];
	task *Object;
	__int16 field_A0;
	__int16 field_A2;
	int field_A4;
};

struct ChaoCharacterBond
{
	char a;
	char b;
	int unknown;
};

struct CHAO_ID
{
	unsigned int gid[2];
	unsigned int id[2];
	unsigned int num;
};

struct AL_GENE
{
	char IsAnalyzed;
	char EggColor;
	unsigned short padding; //???
	CHAO_ID MotherID;
	CHAO_ID FatherID;
	char MotherName[8];
	char FatherName[8];
	char MGroundMotherName[8];
	char MGroundFatherName[8];
	char FGroundMotherName[8];
	char FGroundFatherName[8];
	char Abl[8][2];
	char LifeTime[2];
	char HPos[2];
	char VPos[2];
	char APos[2];
	char Personality[13][2];
	char Taste[2];
	char Tv[2];
	char Music[2];
	unsigned char Color[2];
	char NonTex[2];
	char Jewel[2];
	char Multi[2];
	char Negative[2];
	char EyeScl[2];
	char EyeRatio[2];
	char EyeColor[2];
};


struct  AL_EMOTION
{
	__int16 field_124;
	__int16 Category1Timer;
	__int16 SicknessTimer;
	__int16 Category2Timer;
	char Joy;
	Uint8 Anger;
	Uint8 UrgeToCry;
	char Fear;
	Uint8 Surprise;
	Uint8 Dizziness;
	char Relax;
	char Total;
	__int16 Sleepiness;
	__int16 Tiredness;
	__int16 Hunger;
	__int16 MateDesire;
	__int16 Boredom;
	__int16 Lonely;
	__int16 Tire;
	__int16 Stress;
	__int16 Nourish;
	__int16 Conditn;
	__int16 Energy;
	char Normal_Curiosity;
	char Kindness;
	char CryBaby_Energetic;
	char Naive_Normal;
	char Solitude;
	char Vitality;
	char Glutton;
	char Regain;
	char Skillful;
	char Charm;
	char Chatty;
	char Normal_Carefree;
	char Fickle;
	char FavoriteFruit;
	char field_34;
	char field_35;
	char CoughLevel;
	char ColdLevel;
	char RashLevel;
	char RunnyNoseLevel;
	char HiccupsLevel;
	char StomachAcheLevel;
};


struct AL_KNOWLEDGE_PLAYER
{
	char like;
	char fear;
	unsigned __int16 distance;
	unsigned __int16 meet;
};
struct __declspec(align(4)) AL_KNOWLEDGE_CHAO
{
	CHAO_ID id;
	char like;
	char meetType;
	unsigned __int16 distance;
	unsigned __int16 meet;
};
struct AL_KNOWLEDGE_OTHER
{
	char like;
};
struct AL_KNOWLEDGE
{
	char ArtFlag;
	char DanceFlag;
	char SongFlag;
	char MusicFlag;
	unsigned __int16 SToyFlag;
	unsigned __int16 LToyFlag;
	int KwTimer;
	AL_KNOWLEDGE_PLAYER player[6];
	AL_KNOWLEDGE_CHAO chao[20];
	AL_KNOWLEDGE_OTHER bhv[120];
};
struct AL_PARTS
{
	unsigned int MinimalFlag;
	__int8 MinimalParts[8];
};
struct AL_TIME
{
	char minute;
	char second;
	char frame;
};
struct AL_RACE_PERSONAL_INFO_DX
{
	AL_TIME PersonalRecord[5];
};
struct AL_KNOWLEDGE_DX
{
	AL_KNOWLEDGE_PLAYER player[6];
};

#pragma pack(push, 8)
struct __declspec(align(4)) KarateOpponent
{
	ChaoType ChaoType;
	char EyeType;
	char MouthType;
	char BallType;
	char Headgear;
	char HideFeet;
	char Medal;
	char Color;
	char Monotone;
	char Texture;
	char Shiny;
	SA2BAnimal SA2BArmType;
	SA2BAnimal SA2BEarType;
	SA2BAnimal SA2BForeheadType;
	SA2BAnimal SA2BHornType;
	SA2BAnimal SA2BLegType;
	SA2BAnimal SA2BTailType;
	SA2BAnimal SA2BWingType;
	SA2BAnimal SA2BFaceType;
	char f13;
	__int16 PowerRun;
	__int16 FlySwim;
	__int16 Alignment;
	__int16 Magnitude;
	char Name[7]; //name?
	char unusedPossibly;
	__int16 StatPoints[8];
};
#pragma pack(pop)


struct CHAO_PARAM_GC
{
	char gap_0[18];
	char name[7];
	char field_19;
	char GBATexture;
	char field_1B[5];
	Uint8 Exp[8];
	char Abl[8];
	char Lev[8];
	__int16 Skill[8];
	unsigned __int16 GBAPalette[16];
	char rmsg[16];
	unsigned int runaway;
	char dummy[4];
	ChaoType type;
	char place;
	__int16 like;
	__int16 InKindergarten;
	__int16 age;
	__int16 old;
	__int16 life;
	__int16 LifeMax;
	__int16 nbSucceed;
	CHAO_ID ChaoID;
	int LifeTimer;
	float PowerRun;
	float FlySwim;
	float Alignment;
	int field_B4;
	int field_B8;
	float LastLifeAlignment;
	float EvolutionProgress;
	char gap_C4[13];
	char EyeType;
	char MouthType;
	char BallType;
	char gap_D4[1];
	Uint8 Headgear;
	char HideFeet;
	char Medal;
	unsigned char Color;
	char MonotoneHighlights;
	char Texture;
	char Shiny;
	char EggColor;
	SADXBodyType BodyType;
	char BodyTypeAnimal;
	char field_DF[41];
	__int16 DoctorMedal;
	char KarateInfo;
	char field_10A[13];
	AL_PARTS PartsBTL;
	AL_EMOTION Emotion;
	AL_KNOWLEDGE Knowledge;
	AL_GENE Gene;
	int IsInitializedDX;
	AL_PARTS partsDX;
	char paddingRaceDX[16];//AL_RACE_PERSONAL_INFO_DX raceDX;
	AL_KNOWLEDGE_DX knowledgeDX;

};

struct ChaoUnknownE
{
	char pad[960];
};

struct AnimationInfo
{
	__int16 AnimNum;
	__int16 ModelNum;
	__int16 anonymous_2;
	__int16 NextAnimation;
	float TransitionSpeed;
	float AnimationSpeed;
};

struct CollisionData
{
	__int16 field_0;
	__int16 field_2;
	int field_4;
	NJS_VECTOR some_vector;
	float anonymous_1;
	float anonymous_2;
	float anonymous_3;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
};

struct SonicCharObj2
{
	CharObj2Base base;
	char field_1BC[428];
	__int16 SpindashCounter;
	char field_36A[42];
	NJS_TEXLIST *TextureList;
	ModelIndex *ModelList;
	AnimationIndex *MotionList;
};

struct ModelIndex
{
	int Index;
	NJS_OBJECT *Model;
};

struct AnimationIndex
{
	uint16_t Index;
	uint16_t Count;
	NJS_MOTION *Animation;
};

struct ObjectListHead
{
	int Count;
	ObjectListEntry *List;
};

struct ObjectListEntry
{
	LoadObj LoadFlags;
	char List;
	__int16 ObjectFlags;
	float DistanceMaybe;
	task_exec Function;
	char *Name;
};

struct LevelItemData
{
	int Character;
	void *Name;
	int Mask;
	int Index;
	int field_10;
	void *anonymous_1;
	void *anonymous_2;
	void *anonymous_3;
	int field_20;
};


struct LandTable
{
	__int16 COLCount;
	__int16 ChunkModelCount;
	__int16 field_4;
	__int16 field_6;
	__int16 field_8;
	__int16 field_A;
	float field_C;
	COL *COLList;
	float *field_14;
	char *TextureName;
	NJS_TEXLIST *TextureList;
};

struct COL
{
	NJS_VECTOR Center;
	float Radius;
	NJS_OBJECT *Model;
	int field_14;
	int field_18;
	int Flags;
};

struct StartPosition
{
	__int16 Level;
	__int16 Rotation1P;
	__int16 RotationP1;
	__int16 RotationP2;
	NJS_VECTOR Position1P;
	NJS_VECTOR PositionP1;
	NJS_VECTOR PositionP2;
};


struct LevelHeader
{
	char Name[8];
	int field_8;
	void(__cdecl *Init)();
	void *anonymous_2;
	task_exec subprgmanager;
};

struct ChaoItemStats
{
	__int16 Mood;
	__int16 Belly;
	__int16 Swim;
	__int16 Fly;
	__int16 Run;
	__int16 Power;
	__int16 Stamina;
	__int16 Luck;
	__int16 Intelligence;
	__int16 anonymous_7;
};

struct LevelCutscene
{
	__int16 Level;
	__int16 Character;
	__int16 Cutscene;
};

struct ChaoSegmentData
{
	char *Name;
	void(__cdecl *Prolog)();
	void(__cdecl *Epilog)();
};

struct RELFileInfo
{
	char *Name;
	void(__cdecl *Prolog)();
	void(__cdecl *Epilog)();
};

struct MemManFuncs
{
	AllocatedMem *(__cdecl *Allocate)(int size, const char *file, int line);
	void(__cdecl *Deallocate)(AllocatedMem *, char *, int);
	void *AllocateArray;
	void *Deallocate2;
};

struct ChaoFriendshipEntry 
{
	char UID[8]; //might take long to compare
	char FriendshipValue;
	char Flags;
};

struct CHAO_SAVE_INFO
{
	CHAO_PARAM_GC data;
	
	char Padding2[0x800 - sizeof(CHAO_PARAM_GC)]; //remaining empty space th
};

struct struct_a1
{
	uint8_t byte0;
	uint8_t byte1;
	uint8_t byte2;
	uint8_t byte3;
	uint8_t byte4;
	uint8_t byte5;
	uint8_t byte6;
	uint8_t byte7;
	uint8_t byte8;
	uint8_t byte9;
	uint8_t byteA;
	uint32_t dwordB;
	uint32_t dwordF;
	uint8_t f13[1];
	uint16_t word14;
	uint8_t f16[14];
	char char24;
	char gap_25[14];
	char field_33;
};

struct TexPackInfo
{
	char *TexName;
	NJS_TEXLIST *TexList;
};

struct LevelRankScores
{
	__int16 Level;
	__int16 DRank;
	__int16 CRank;
	__int16 BRank;
	__int16 ARank;
};

struct struct_this
{
	uint8_t f0[2];
	uint8_t Rank;
	char Perfect;
	char gap_4[4];
	uint32_t TimeBonus;
	uint32_t Score;
};

struct MinSec
{
	char Minutes;
	char Seconds;
};

struct LevelRankTimes
{
	char Level;
	MinSec DRank;
	MinSec CRank;
	MinSec BRank;
	MinSec ARank;
};

struct BlackMarketItem
{
	ChaoItemCategory Category;
	Sint8 Type;
};

struct LevelEndPosition
{
	__int16 Level;
	__int16 Mission2YRotation;
	__int16 Mission3YRotation;
	__int16 field_6;
	NJS_VECTOR Mission2Position;
	NJS_VECTOR Mission3Position;
};

struct CharacterSoundBank
{
	int Character;
	char *Name;
	void *anonymous_1;
};

struct CharacterVoiceBank
{
	char anonymous_0;
	char Character;
	char anonymous_2[2];
	void *anonymous_3;
	void *anonymous_4;
	void *anonymous_5;
};

struct DeathZone
{
	int Flags;
	NJS_OBJECT *Model;
};

struct AnalogThing
{
	Angle direction;
	float magnitude;
};

struct LoopPoint
{
	__int16 XRot;
	__int16 YRot;
	float Distance;
	NJS_VECTOR Position;
};

struct LoopHead
{
	__int16 anonymous_0;
	__int16 Count;
	float TotalDistance;
	LoopPoint *Points;
	task_exec Object;
};

struct EmeManThing
{
	char byte0;
	char byte1;
	char byte2;
	char byte3;
	NJS_VECTOR v;
};

struct KnucklesCharObj2
{
	CharObj2Base base;
	char field_1BC[564];
	NJS_TEXLIST *TextureList;
	NJS_TEXLIST *EffectTextureList;
	ModelIndex *ModelList;
	AnimationIndex *MotionList;
	char field_400[32];
};

struct EggmanCharObj2
{
	CharObj2Base base;
	char field_1BC[420];
	NJS_TEXLIST *TextureList;
	ModelIndex *ModelList;
	AnimationIndex *MotionList;
};

struct MechEggmanCharObj2
{
	CharObj2Base base;
	char field_1BC[128];
	float field_23C;
	char field_240[4];
	float field_244;
	int field_248;
	char field_24C[8];
	float field_254;
	float field_258;
	float field_25C;
	char field_260[120];
	float field_2D8;
	float field_2DC;
	float field_2E0;
	float field_2E4;
	float field_2E8;
	float field_2EC;
	float field_2F0;
	float field_2F4;
	float field_2F8;
	char field_2FC[96];
	char field_35C;
	char field_35D;
	char field_35E;
	char field_35F;
	char field_360;
	char field_361[3];
	__int16 field_364;
	__int16 field_366;
	__int16 field_368;
	__int16 field_36A;
	__int16 field_36C;
	__int16 field_36E;
	char field_370[4];
	float field_374;
	char field_378[8];
	int field_380;
	char field_384[12];
	float field_390;
	float field_394;
	float field_398;
	int field_39C;
	char field_3A0[132];
	int field_424;
	int field_428;
	int field_42C;
	int field_430;
	int field_434;
	float field_438;
	float field_43C;
	float field_440;
	NJS_TEXLIST *CommonTextureList;
	NJS_TEXLIST *TextureList;
	ModelIndex *ModelList;
	AnimationIndex *MotionList;
};

struct TailsCharObj2
{
	CharObj2Base base;
	char field_1BC[500];
	NJS_TEXLIST *TextureList;
	ModelIndex *ModelList;
	AnimationIndex *MotionList;
	char field_3BC[36];
};

struct SuperSonicCharObj2
{
	CharObj2Base base;
	char field_1BC[436];
	NJS_TEXLIST *TextureList;
	ModelIndex *ModelList;
	AnimationIndex *MotionList;
};

struct EnemyField38
{
	float f0;
	char field_4[24];
	int field_1C;
	char field_20[36];
	uint8_t byte44;
	uint8_t byte45;
	char field_46;
	char field_47;
	__int16 field_48;
	__int16 field_4A;
	uint16_t word4C;
	uint8_t f4E[6];
	float float54;
	float dword58;
	float dword5C;
	float dword60;
	char f64[52];
	float field_98;
	char field_9C[36];
	float floatC0;
	uint8_t fC4[4];
	float floatC8;
	float floatCC;
	float floatD0;
	float floatD4;
	float floatD8;
	float floatDC;
	float floatE0;
	float floatE4;
	uint32_t dwordE8;
	float fEC;
	float field_F0;
	char field_F4[208];
	int field_1C4;
	uint32_t dword1C8;
	uint8_t f1CC[8];
	uint32_t dword1D4;
	float float1D8;
	float float1DC;
	int field_1E0;
	char field_1E4[15];
	char field_1F3;
	char field_1F4[4];
	int field_1F8;
	int field_1FC;
	char field_200[16];
};

struct EventFileHeader
{
	void *field_0;
	NJS_TEXLIST *field_4;
	int field_8;
	void *field_C;
	void *field_10;
	void *field_14;
	void *field_18;
	NJS_OBJECT **field_1C;
	void *field_20;
	void *field_24;
	int field_28;
};

struct ControlShadowCharObj2
{
	uint8_t byte0;
	uint8_t byte1;
	uint16_t word2;
	uint16_t word4;
	uint8_t f6[2];
};

struct UserConfigData
{
	int Fullscreen;
	int Width;
	int Height;
	int RefreshRate;
	int Language;
	int Display;
	int Res;
	int Analytics;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
	int Rumble;
};

struct LaunchConfig
{
	LaunchConfig_vtable *vtable;
	UserConfigData data;
	char gap_38[451];
	char field_1FB;
};

struct LaunchConfig_vtable
{
	int sub_4025E0;
};

struct struct_v1
{
	uint32_t dword0;
	uint8_t f4[4];
	uint32_t dword8;
	uint8_t byteC;
	uint8_t fD[35];
	uint32_t dword30;
	uint32_t dword34;
	uint32_t dword38;
};

struct PDS_PERIPHERALINFO
{
	Uint32 type;
	Uint32 reserved[3];
	Uint8 is_root;
	Uint8 area_code;
	Uint8 connector_dir[2];
	char product_name[32];
	char license[64];
	Uint16 stdby_pow;
	Uint16 max_pow;
};

struct PDS_PERIPHERAL
{
	Uint32 id;
	Uint32 support;
	Uint32 on;
	Uint32 off;
	Uint32 press;
	Uint32 release;
	Uint16 r;
	Uint16 l;
	Sint16 x1;
	Sint16 y1;
	Sint16 x2;
	Sint16 y2;
	char *name;
	void *extend;
	Uint32 old;
	PDS_PERIPHERALINFO *info;
};

struct ButtonMapping
{
	int XInput;
	int Dreamcast;
};

struct XInputMapping
{
	int dword_8AE160;
	int dword_8AE164;
};

struct PDS_BASE
{
	int off;
	int press;
	int release;
	__int16 R;
	__int16 L;
	__int16 analog_l_x;
	__int16 analog_l_y;
	__int16 analog_r_x;
	__int16 analog_r_y;
	int name;
	int extend;
	int old;
	PDS_PERIPHERALINFO *info;
	int field_28;
	int field_2C;
	int field_30;
};

struct FogData
{
	float field_0;
	float field_4;
	float field_8;
	int field_C;
};

struct SaveFileStruct
{
	char field_0[842];
};



struct DebugFunction
{
	void(__cdecl *Function)(void *_this);
	int Enabled;
	const char *Name;
};

struct ChaoUnknownC
{
	char gap_0[2];
	__int16 field_2;
	char gap_4[8];
	int field_C;
	int field_10;
	char gap_14[16];
	char field_24[72];
	char gap_6C[516];
	float field_270;
	NJS_VECTOR vector;
	char gap_280[384];
	char field_400;
};

struct MotionTableEntry
{
	__int16 field_0;
	__int16 field_2;
	int field_4;
	int field_8;
	float field_C;
	float field_10;
	float field_14;
	__int16 field_18;
	__int16 field_1A;
	float field_1C;
	float field_20;
	float field_24;
	float field_28;
	int field_2C;
	int field_30;
	float field_34;
};

struct ALO_ChaosDriveExecutor_Data1
{
	taskwk entity;
	char field_30;
	char gap_31[3];
	int field_34;
	char gap_38[4];
	int field_3C;
	int field_40;
	char gap_44[4];
	int field_48;
	int field_4C;
	int field_50;
};

struct AL_MinimalExecutor_Data1
{
	taskwk entity;
	int field_30;
	int field_34;
	MOTION_CTRL field_38;
	MotionTableAction field_84;
	MotionTableAction field_A0;
	MotionTableAction field_BC;
	MotionTableAction field_D8;
	int field_F4;
};

struct ALO_EntranceElevatorExecutor_Data2
{
	int field_0;
	int field_4;
	NJS_VECTOR Position;
	int field_14;
	int field_18;
	int field_1C;
	NJS_VECTOR field_20;
	int field_2C;
	int field_30;
	int field_34;
};

struct CharSelectThing
{
	char Costume;
	char CostumeUnlocked;
	char gap_2;
	char Visible;
	char gap_4[16];
};



struct MenuData
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
};


struct al_confirmload_data2
{
	uint32_t state;
	uint32_t dword4;
	uint32_t dword8;
	uint32_t dwordC;
	uint32_t dword10;
	uint32_t dword14;
	char *data_ptr;
	uint32_t dword1C;
	uint32_t dword20;
};

struct AL_GBAManagerExecutor_Data
{
	int field_0;
	int field_4;
	int field_8;
	task *ObjectPtr;
	char field_10[4228];
	char gap_1094[12691];
	char field_4227;
};

struct ChaoEggData
{
	uint32_t dword0;
	char gap_4[4];
	uint32_t dword8;
	uint32_t dwordC;
	uint8_t gap10[4];
	uint32_t dword14;
	float float18;
	float float1C;
	float field_20;
	uint32_t dword24;
	int field_28;
};

struct ProbablyShaderRelated
{
	int field_0;
	int field_4;
	int HorizontalResolution;
	int field_C;
	int field_10;
	int field_14;
	int RefreshRate;
	int Display;
	int field_20;
	int field_24;
	int field_28;
};

struct __declspec(align(8)) struct_a2
{
	void *vtbl;
	int field_4;
	int field_8;
	int field_C;
	uint32_t dword10;
	uint32_t dword14;
	uint8_t byte18;
	char gap_19[3];
	uint32_t DisplayHeight;
	uint32_t RefreshRate;
	uint32_t dword24;
	uint32_t dword28;
	uint32_t dword2C;
	uint8_t byte30;
	char gap_31[3];
	uint32_t dword34;
	uint32_t dword38;
	uint8_t gap3C[32];
	uint32_t dword5C;
	uint8_t gap60[788];
	uint8_t byte374;
	char gap_375[3];
	uint32_t dword378;
	uint32_t dword37C;
};

#pragma pack(pop)
