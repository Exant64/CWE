#pragma once
#include "al_face.h"
#include "al_motion.h"
#include "al_misc.h"
#include "c_colli.h"
#include <al_world.h>
#include "al_emotion.h"
#include "al_parameter.h"
#include "al_behavior/al_intention.h"
#include "move.h"

#define CHAOWK(a) (a->Data1.Chao)
#define GET_CHAOWK(tp) ((chaowk*)tp->Data1.Chao)
#define GET_CHAOPARAM(tp) (GET_CHAOWK(tp)->pParamGC)

enum AL_PARAM_FLAG
{
	AL_PARAM_FLAG_NAME_NEW = BIT_0,
	AL_PARAM_FLAG_OLD_GUEST_CHECK = BIT_1,
	AL_PARAM_FLAG_PARTS_CONVERSION = BIT_2,
	AL_PARAM_FLAG_ACCESSORIES_NEW = BIT_3,

	AL_PARAM_FLAG_NEWBORN = BIT_0 | BIT_1 | BIT_2 | BIT_3
};

enum ChaoBehaviour : __int32
{
	ChaoBehaviour_RACEWAIT = 0x00538BE0, //Pose before the Race Start
	ChaoBehaviour_SLEEP = 0x0054EF10,
	ChaoBehaviour_GETTINGTIRED = 0x0054F980,
	ChaoBehaviour_TIRED = 0x0054FCF0, //Why the timer goes up on this one?
	ChaoBehaviour_MOVE = 0x0055C3C0, //PostureChange
	ChaoBehaviour_TIRE = 0x0055C430, //Chao Sit
	ChaoBehaviour_BROOMSTICK = 0x0055DD10,
	ChaoBehaviour_POGOSTICK = 0x0055E6D0,
	ChaoBehaviour_HANDING = 0x005606D0,
	ChaoBehaviour_EATING = 0x005607C0,
	ChaoBehaviour_PICKUPFRUIT = 0x005613C0,
	ChaoBehaviour_SWIM = 0x00561FB0,
	ChaoBehaviour_SUPERSWIM = 0x00562040, //Chao Swim but his Swim is over 700 points
	ChaoBehaviour_SWIMIDLE = 0x00562160,
	ChaoBehaviour_TRIP = 0x00562D50,
	ChaoBehaviour_CLIMB = 0x00562EB0,
	ChaoBehaviour_FLY = 0x005630C0, //Fly, my Child
	ChaoBehaviour_GOTOPLAYER = 0x00563250, //Player used Whistle
	ChaoBehaviour_BEINGCALLED = 0x005634D0, //Player used Whistle
	ChaoBehaviour_GOTOBALL = 0x00563830,
	ChaoBehaviour_GOTDAMAGE = 0x00563EB0,
	ChaoBehaviour_GOTTHROWND = 0x00564320,
	ChaoBehaviour_JUMPSCARED = 0x005647B0, //From the Jack n Box Toy
	ChaoBehaviour_GETPET = 0x005648A0,
	ChaoBehaviour_THINK = 0x00565BE0,  //This also kill PrevFunc so that's gonna suck to detect Previous Behaviour
	ChaoBehaviour_GOTANIMAL = 0x00566DF0,
	ChaoBehaviour_THINKSEED = 0x00567710,
	ChaoBehaviour_WALKWITHSEED = 0x00567830,
	ChaoBehaviour_FOUNDSEEDSPOT = 0x00567B10,
	ChaoBehaviour_WATERCAN = 0x005680F0,
	ChaoBehaviour_WATERCANDANCE = 0x00568460,
	ChaoBehaviour_COMEOUTEGG = 0x005685F0,
	ChaoBehaviour_HOLDINGITEM = 0x00569340,
	ChaoBehaviour_NOTICEITEM = 0x0056B480,
	ChaoBehaviour_PUTHAT = 0x00569430,
	ChaoBehaviour_THROWITEM = 0x00569550,
	ChaoBehaviour_GOTOITEM = 0x0056BA80,
	ChaoBehaviour_WATCHINGTV = 0x00598890,
	ChaoBehaviour_WATCHINGRADIO = 0x00598B90,
	ChaoBehaviour_RASH = 0x0059A0B0,
	ChaoBehaviour_WATCHINGSING = 0x0059B3D0,
	ChaoBehaviour_MATEWALK = 0x0059BF30,
	ChaoBehaviour_WATCHINGDANCE = 0x0059CDA0,
	ChaoBehaviour_MARACAS = 0x0059D5A0,
	ChaoBehaviour_FLUTE = 0x0059DD10,
	ChaoBehaviour_TRUMPET = 0x0059E120,
	ChaoBehaviour_WATCHINGMUSIC = 0x0059E710,
	ChaoBehaviour_WANTTHEHOLDFRUIT = 0x0059EA00, //The chao beg the player who is holding a fruit
	ChaoBehaviour_GOTOHOLDFRUIT = 0x0059ED60, //The chao go to the player who is holding a fruit
	ChaoBehaviour_HUNGRY = 0x0059ED60,
	ChaoBehaviour_STARVING = 0x0059EF40,
	ChaoBehaviour_FEAR = 0x0059F350,
	ChaoBehaviour_SADNESS = 0x0059F970,
	ChaoBehaviour_URGETOCRY = 0x0059FA50,
	ChaoBehaviour_URGETOCRY2 = 0x0059FCA0, //Oh God there's tears
	ChaoBehaviour_DIZZY = 0x005A00A0,
	ChaoBehaviour_HEROHI = 0x005A03B0,
	ChaoBehaviour_LOVETHEPLAYER = 0x005A0820,
	ChaoBehaviour_COMETOPLAYER = 0x005A0960,
	ChaoBehaviour_JUMP = 0x005A0F40,
	ChaoBehaviour_ANGER = 0x005A1460,
	ChaoBehaviour_CHAOHATE = 0x005A1800, //"Baaah!"
	ChaoBehaviour_SITTHINK = 0x005A1BA0,
	ChaoBehaviour_UNICORN = 0x005A1FC0,
	ChaoBehaviour_GORILLA = 0x005A2010,
	ChaoBehaviour_WARTHOG = 0x005A2310,
	ChaoBehaviour_BUNNY = 0x005A2860,
	ChaoBehaviour_CRAWL = 0x005A3280,
	ChaoBehaviour_WALK = 0x005A3380,
	ChaoBehaviour_RUN1 = 0x005A3460,
	ChaoBehaviour_RUN2 = 0x005A3580,
	ChaoBehaviour_DRAWING = 0x005AB2C0,
};

enum ChaoIcon : __int8
{
	ChaoIcon_Normal = 0x0,
	ChaoIcon_ExclamationMark = 0x1,
	ChaoIcon_QuestionMark = 0x3,
	ChaoIcon_Heart = 0x4,
	ChaoIcon_Spiral = 0x5,
};


enum eHEAD_PARTS
{
	OBAKE_HEAD_PARTS_NONE = 0x0,
	OBAKE_HEAD_PARTS_PUMPKIN = 0x1,
	OBAKE_HEAD_PARTS_SKULL = 0x2,
	OBAKE_HEAD_PARTS_APPLE = 0x3,
	OBAKE_HEAD_PARTS_BUCKET = 0x4,
	OBAKE_HEAD_PARTS_CAN = 0x5,
	OBAKE_HEAD_PARTS_CDBOX = 0x6,
	OBAKE_HEAD_PARTS_FLOWERPOT = 0x7,
	OBAKE_HEAD_PARTS_PAPERBAG = 0x8,
	OBAKE_HEAD_PARTS_STEWPAN = 0x9,
	OBAKE_HEAD_PARTS_STUMP = 0xA,
	OBAKE_HEAD_PARTS_WMELON = 0xB,
	OBAKE_HEAD_PARTS_WOOL_A = 0xC,
	OBAKE_HEAD_PARTS_WOOL_B = 0xD,
	OBAKE_HEAD_PARTS_WOOL_C = 0xE,
	OBAKE_HEAD_PARTS_TEETHINGRING = 0xF,
	OBAKE_HEAD_PARTS_EGG = 0x10,
	OBAKE_HEAD_PARTS_EGG_Yellow = 0x11,
	OBAKE_HEAD_PARTS_EGG_White = 0x12,
	OBAKE_HEAD_PARTS_EGG_Brown = 0x13,
	OBAKE_HEAD_PARTS_EGG_SkyBlue = 0x14,
	OBAKE_HEAD_PARTS_EGG_Pink = 0x15,
	OBAKE_HEAD_PARTS_EGG_Blue = 0x16,
	OBAKE_HEAD_PARTS_EGG_Gray = 0x17,
	OBAKE_HEAD_PARTS_EGG_Green = 0x18,
	OBAKE_HEAD_PARTS_EGG_Red = 0x19,
	OBAKE_HEAD_PARTS_EGG_AppleGreen = 0x1A,
	OBAKE_HEAD_PARTS_EGG_Purple = 0x1B,
	OBAKE_HEAD_PARTS_EGG_Orange = 0x1C,
	OBAKE_HEAD_PARTS_EGG_Black = 0x1D,
	OBAKE_HEAD_PARTS_EGG_BlendYellow = 0x1E,
	OBAKE_HEAD_PARTS_EGG_BlendWhite = 0x1F,
	OBAKE_HEAD_PARTS_EGG_BlendBrown = 0x20,
	OBAKE_HEAD_PARTS_EGG_BlendSkyBlue = 0x21,
	OBAKE_HEAD_PARTS_EGG_BlendPink = 0x22,
	OBAKE_HEAD_PARTS_EGG_BlendBlue = 0x23,
	OBAKE_HEAD_PARTS_EGG_BlendGray = 0x24,
	OBAKE_HEAD_PARTS_EGG_BlendGreen = 0x25,
	OBAKE_HEAD_PARTS_EGG_BlendRed = 0x26,
	OBAKE_HEAD_PARTS_EGG_BlendAppleGreen = 0x27,
	OBAKE_HEAD_PARTS_EGG_BlendPurple = 0x28,
	OBAKE_HEAD_PARTS_EGG_BlendOrange = 0x29,
	OBAKE_HEAD_PARTS_EGG_BlendBlack = 0x2A,
	OBAKE_HEAD_PARTS_EGG_ShinyNormal = 0x2B,
	OBAKE_HEAD_PARTS_EGG_ShinyYellow = 0x2C,
	OBAKE_HEAD_PARTS_EGG_ShinyWhite = 0x2D,
	OBAKE_HEAD_PARTS_EGG_ShinyBrown = 0x2E,
	OBAKE_HEAD_PARTS_EGG_ShinySkyBlue = 0x2F,
	OBAKE_HEAD_PARTS_EGG_ShinyPink = 0x30,
	OBAKE_HEAD_PARTS_EGG_ShinyBlue = 0x31,
	OBAKE_HEAD_PARTS_EGG_ShinyGray = 0x32,
	OBAKE_HEAD_PARTS_EGG_ShinyGreen = 0x33,
	OBAKE_HEAD_PARTS_EGG_ShinyRed = 0x34,
	OBAKE_HEAD_PARTS_EGG_ShinyAppleGreen = 0x35,
	OBAKE_HEAD_PARTS_EGG_ShinyPurple = 0x36,
	OBAKE_HEAD_PARTS_EGG_ShinyOrange = 0x37,
	OBAKE_HEAD_PARTS_EGG_ShinyBlack = 0x38,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendYellow = 0x39,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendWhite = 0x3A,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendBrown = 0x3B,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendSkyBlue = 0x3C,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendPink = 0x3D,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendBlue = 0x3E,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendGray = 0x3F,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendGreen = 0x40,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendRed = 0x41,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendAppleGreen = 0x42,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendPurple = 0x43,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendOrange = 0x44,
	OBAKE_HEAD_PARTS_EGG_ShinyBlendBlack = 0x45,
	OBAKE_HEAD_PARTS_EGG_Gold = 0x46,
	OBAKE_HEAD_PARTS_EGG_Silver = 0x47,
	OBAKE_HEAD_PARTS_EGG_Ruby = 0x48,
	OBAKE_HEAD_PARTS_EGG_Sapphire = 0x49,
	OBAKE_HEAD_PARTS_EGG_Emerald = 0x4A,
	OBAKE_HEAD_PARTS_EGG_Amethyst = 0x4B,
	OBAKE_HEAD_PARTS_EGG_Aquamarine = 0x4C,
	OBAKE_HEAD_PARTS_EGG_Garnet = 0x4D,
	OBAKE_HEAD_PARTS_EGG_Onyx = 0x4E,
	OBAKE_HEAD_PARTS_EGG_Peridot = 0x4F,
	OBAKE_HEAD_PARTS_EGG_Topaz = 0x50,
	OBAKE_HEAD_PARTS_EGG_Pearl = 0x51,
	OBAKE_HEAD_PARTS_EGG_Env0 = 0x52,
	OBAKE_HEAD_PARTS_EGG_Env1 = 0x53,
	OBAKE_HEAD_PARTS_EGG_Env2 = 0x54,
	OBAKE_HEAD_PARTS_END = 0x55,
};

enum eFRUIT_KIND
{
	FRUIT_NEUT_LOCAL = 0,
	FRUIT_HERO_LOCAL = 1,
	FRUIT_DARK_LOCAL = 2,
	FRUIT_MORIMORI = 3,
	FRUIT_PAKUPAKU = 4,
	FRUIT_HERO = 5,
	FRUIT_DARK = 6,
	FRUIT_MARU = 7,
	FRUIT_SANKAKU = 8,
	FRUIT_SIKAKU = 9,
	FRUIT_HEART = 10,
	FRUIT_CHAO = 11,
	FRUIT_ORIKOU = 12,
	FRUIT_GBA_A = 13,
	FRUIT_GBA_B = 14,
	FRUIT_GBA_C = 15,
	FRUIT_GBA_D = 16,
	FRUIT_GBA_E = 17,
	FRUIT_GBA_F = 18,
	FRUIT_GBA_G = 19,
	FRUIT_KINOKO_A = 20,
	FRUIT_KINOKO_B = 21,
	FRUIT_MINTCANDY = 22,
	FRUIT_GRAPE = 23,
	FRUIT_SPOILED_NEUT,
	FRUIT_SPOILED_HERO,
	FRUIT_SPOILED_DARK,
	FRUIT_SPOILED_STRONG,
	FRUIT_SPOILED_TASTY,
	FRUIT_HYPER_SWIM,
	FRUIT_HYPER_FLY,
	FRUIT_HYPER_RUN,
	FRUIT_HYPER_POWER,
	FRUIT_SHINY,
	FRUIT_CAKE,
	FRUIT_ORANGE,
	FRUIT_END
};
enum 
{
	TASTE_LS_DT = 0x0,
	TASTE_LS_DC = 0x1,
	TASTE_LT_DS = 0x2,
	TASTE_LT_DC = 0x3,
	TASTE_LC_DS = 0x4,
	TASTE_LC_DT = 0x5,
};
enum eAL_STOY
{
	AL_STOY_GARAGARA = 0x0,
	AL_STOY_CAR = 0x1,
	AL_STOY_BOOK = 0x2,
	AL_STOY_CRACKER = 0x3,
	AL_STOY_DOLL = 0x4,
	AL_STOY_HOUKI = 0x5,
	AL_STOY_FIREWORK = 0x6,
	AL_STOY_HOPPING = 0x7,
	AL_STOY_CRAYON = 0x8,
	AL_STOY_SYABON = 0x9,
	AL_STOY_SCOP = 0xA,
	AL_STOY_JYOURO = 0xB,
	NB_AL_STOY = 0xC,
};
enum eAL_LTOY
{
	AL_LTOY_TV = 0x0,
	AL_LTOY_RADICASE = 0x1,
	AL_LTOY_BOX = 0x2,
	AL_LTOY_BALL_N = 0x3,
	AL_LTOY_BALL_H = 0x4,
	AL_LTOY_BALL_D = 0x5,
	AL_LTOY_HORSE = 0x6,
	AL_LTOY_UKIWA = 0x7,
	AL_LTOY_DUCK = 0x8,
	AL_LTOY_KANOKE = 0x9,
	AL_LTOY_PIANO = 0xA,
	AL_LTOY_ORGAN = 0xB,
	NB_AL_LTOY = 0xC,
};
enum : Uint8
{
	CI_KIND_AL_CHAO = 0x81,
	CI_KIND_AL_CHAO_FIELD = 0x82,
	CI_KIND_AL_FRUIT = 0x83,
	CI_KIND_AL_FRUIT_FIELD = 0x84,
	CI_KIND_AL_SEED = 0x85,
	CI_KIND_AL_SEED_FIELD = 0x86,
	CI_KIND_AL_MINIMAL = 0x87,
	CI_KIND_AL_MINIMAL_FIELD = 0x88,
	CI_KIND_AL_TREE = 0x89,
	CI_KIND_AL_TREE_FIELD = 0x8A,
	CI_KIND_AL_TOY = 0x8B,
	CI_KIND_AL_TOY_FIELD = 0x8C,
	CI_KIND_AL_EGG = 0x8D,
	CI_KIND_AL_EGG_FIELD = 0x8E,
	CI_KIND_AL_TRASH = 0x8F,
	CI_KIND_AL_TRASH_FIELD = 0x90,
	CI_KIND_AL_DAKKO_FIELD = 0x91,
	CI_KIND_AL_NADE_FIELD = 0x92,
	CI_KIND_AL_OBAKE_HEAD = 0x93,
	CI_KIND_AL_CDR = 0x94,
	CI_KIND_AL_ACCESSORY = 0x94,
	CI_KIND_AL_PUNCH = 0x95,
	CI_KIND_AL_SHADOW = 0x96,
	CI_KIND_AL_ONARA = 0x97,
	CI_KIND_AL_SOUND = 0x98,
	CI_KIND_AL_HELLO = 0x99,
	CI_KIND_AL_TOBIKOMI = 0x9A,
	CI_KIND_AL_REQUEST_WATER = 0x9B,
	CI_KIND_AL_NOISE = 0x9C,
	CI_KIND_AL_CONTACT = 0x9D,
	CI_KIND_AL_ACTION = 0x9E,
	CI_KIND_AL_SONG = 0x9F,
	CI_KIND_AL_BALL = 0xA0,
	CI_KIND_AL_FIRE = 0xA1,
	CI_KIND_AL_BIKKURI = 0xA2,
	CI_KIND_AL_BOX = 0xA3,
	CI_KIND_AL_WALL = 0xA4,
	CI_KIND_AL_RANDOM_MUSIC = 0xA5,
	CI_KIND_AL_DANCE = 0xA6,
	CI_KIND_AL_SING = 0xA7,
	CI_KIND_AL_SING_ONCHI = 0xA8,
	CI_KIND_AL_RAKUGAKI = 0xA9,
	CI_KIND_AL_FINISH = 0xAA,
	CI_KIND_AL_NEST = 0xAB,
	CI_KIND_AL_PLANT_POS = 0xAC,
	CI_KIND_AL_WATER = 0xAD,
	CI_KIND_AL_THIRSTY = 0xAE,
	CI_KIND_AL_TOUCH = 0xAF,
	CI_KIND_AL_SWITCH = 0xB0,
	CI_KIND_AL_CHANGE = 0xB1,
	CI_KIND_AL_SET_RACE_TREE = 0xB2,
	CI_KIND_AL_SET_RACE_GLIDE = 0xB3,
	CI_KIND_AL_SET_RACE_JUMP = 0xB4,
	CI_KIND_AL_SET_RACE_LAKE = 0xB5,
	CI_KIND_AL_SET_RACE_KARIMONO = 0xB6,
	CI_KIND_AL_SET_RACE_KOWAI = 0xB7,
	CI_KIND_AL_SET_RACE_GOAL = 0xB8,
	CI_KIND_AL_SET_RACE_ENABLE_TURN = 0xB9,
	CI_KIND_AL_SET_RACE_ALONG_LANE = 0xBA,
	CI_KIND_AL_SET_RACE_REMEMBER_POS = 0xBB,
	CI_KIND_AL_SET_RACE_THINK_POS = 0xBC,
	CI_KIND_AL_SET_RACE_PARALLEL = 0xBD,
	CI_KIND_AL_SET_RACE_GOAL_BANZAI = 0xBE,
	CI_KIND_AL_SET_RACE_PATHCOLLI = 0xBF,
	CI_KIND_AL_SET_RACE_POLYCOLLI = 0xC0,
	CI_KIND_AL_SET_RACE_EACHCOLLI = 0xC1,
	CI_KIND_AL_LOG = 0xC2,
	CI_KIND_AL_ROCK = 0xC3,
	CI_KIND_AL_HOLL = 0xC4,
	CI_KIND_AL_KANI = 0xC5,
	CI_KIND_AL_ANSWER_POS = 0xC6,
	CI_KIND_AL_ANSWER_OBJ = 0xC7,
	CI_KIND_AL_GOOD = 0xC8,
	CI_KIND_AL_TV = 0xC9,
	CI_KIND_AL_RADICASE = 0xCA,
	CI_KIND_AL_CLIMB = 0xCB,
	CI_KIND_AL_FURIMUKI = 0xCC,
	CI_KIND_AL_STAGE_CHANGER = 0xCD,
	CI_KIND_AL_SPECIAL = 0xCE,
	CI_KIND_AL_MAYU_GROW_FIELD = 0xD0,
	CI_KIND_AL_MAYU_DEATH_FIELD = 0xD1,
	CI_KIND_AL_MAYU_SUCCEED_FIELD = 0xD2,
	CI_KIND_AL_PIANO = 0xE1,
	CI_KIND_AL_RANDOM_STOY = 0xE6,
	CI_KIND_END = 0xFF,
};

enum
{
	BHV_RET_CONTINUE = 0x0,
	BHV_RET_FINISH = 0x1,
	BHV_RET_THINK = 0x2,
	BHV_RET_BREAK = 0x3,
};

enum
{
	G_PER_CURIOSITY = 0x0,
	G_PER_KINDNESS = 0x1,
	G_PER_AGRESSIVE = 0x2,
	G_PER_SLEEPY_HEAD = 0x3,
	G_PER_SOLITUDE = 0x4,
	G_PER_VITALITY = 0x5,
	G_PER_GLUTTON = 0x6,
	G_PER_REGAIN = 0x7,
	G_PER_SKILLFUL = 0x8,
	G_PER_CHARM = 0x9,
	G_PER_CHATTY = 0xA,
	G_PER_CALM = 0xB,
	G_PER_FICKLE = 0xC,
	NB_G_PER = 0xD,
};
enum ALW_KIND
{
	ALW_KIND_NONE = 0x0,
	ALW_KIND_CDR = 0x1,
	ALW_KIND_TV = 0x2,
	ALW_KIND_RADICASE = 0x3,
	ALW_KIND_BALL = 0x4,
	ALW_KIND_PIANO = 0x5,
	ALW_KIND_ORGAN = 0x6,
	ALW_KIND_HORSE = 0x7,
	ALW_KIND_BOX = 0x8,
	//CWE STARTS HERE
	ALW_KIND_BOAT = 0x9,
	ALW_KIND_UKIWA = 10,
	NB_ALW_KIND
};

struct POS_XZ
{
	int x;
	int z;
};
struct LAND_ATTR_INDEX
{
	int nbIndex;
	POS_XZ pos[512];
};
DataArray(LAND_ATTR_INDEX, stru_1A15938, 0x1A15938, 15);

ThiscallFunctionPointer(signed int, AL_MoveHoldingObject, (ObjectMaster* a1), 0x56CFF0);
ObjectMaster* __cdecl AL_GetFoundToyTask(ObjectMaster* a1);
void AL_ChaoAccessoryConversion(ChaoDataBase* pParam);
void __cdecl AL_SetItem(ObjectMaster* a1, int a2, NJS_OBJECT* model, NJS_TEXLIST* texlist);
FastcallFunctionPointer(signed int, AL_TraceHoldingPosition, (int a1, ObjectMaster* a2), 0x56D170);
ObjectMaster* sub_55A920(int a1, NJS_VECTOR* a2, ObjectMaster* parent, Uint8 a4, float a5);
void sub_54A690(ObjectMaster* a1);
void LoadCocoon(ObjectMaster* a1, char a2);
void ALOField_Load(ObjectMaster* a1, Uint8 a2, NJS_VECTOR* a3, float a4, int timer);

ObjectMaster* GetClosestChao(ObjectMaster* a1);
void AL_IconSet(ObjectMaster* a4, char a2, int a3);

void AL_GetRandomAttrPos_0(ObjectMaster* a1);	
typedef ChaoData1 chaowk;
typedef int(__cdecl* BHV_FUNC)(ObjectMaster*);
void sub_54A730(ObjectMaster* a1);
extern "C" __declspec(dllexport) signed int __cdecl ALBHV_WearAccessory(ObjectMaster* a1);
void __cdecl AL_Brake(ObjectMaster* a1, float a2);
void AL_SetBehavior(ObjectMaster* a1, BHV_FUNC a2);
void AL_SetNextBehavior(ObjectMaster* a1, BHV_FUNC a2);
void __cdecl AL_ForwardAcc(ObjectMaster* a1, float a2);
BHV_FUNC AL_GetBehavior(ObjectMaster* a1);
void Chao_Init();

void ColorEggModel(NJS_CNK_MODEL* a1, int a2);

//todo: move this and al_global struct to its own header someday
#define GET_GLOBAL() (&ChaoGlobal)
