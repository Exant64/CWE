#pragma once

// SA2 Enums
#define makemask(enumname,membername) enumname##_##membername = (1 << enumname##Bits_##membername)
#define makemasks(enumname,membername) enumname##s_##membername = (1 << enumname##Bits_##membername)

enum
{
    PLNO_SONIC,                     /* sonic the hedgehog                                       */
    PLNO_SHADOW,                    /* shadow the hedgehog                                      */
    PLNO_MILES,                     /* miles "tails" prower                                     */
    PLNO_EGGMAN,                    /* eggman                                                   */
    PLNO_KNUCKLES,                  /* knuckles the echidna                                     */
    PLNO_ROUGE,                     /* rouge the bat                                            */
    PLNO_TAILS_WALKER,              /* tails walker                                             */
    PLNO_EGG_WALKER,                /* egg walker                                               */
    PLNO_AMY,                       /* amy rose         : basechar = sonic                      */
    PLNO_SUPER_SONIC,               /* super sonic                                              */
    PLNO_SUPER_SHADOW,              /* super shadow                                             */
    PLNO_DUMMY,                     /* dummy                                                    */
    PLNO_METAL_SONIC,               /* metal sonic      : basechar = shadow                     */
    PLNO_CHAO_WALKER,               /* chao walker      : basechar = tails walker               */
    PLNO_DARK_WALKER,               /* dark walker      : basechar = egg walker                 */
    PLNO_TICAL,                     /* tikal            : basechar = knuckles                   */
    PLNO_CHAOS0,                    /* chaos 0          : basechar = rouge                      */

    NB_PLNO,                        /* enum count                                               */
};

enum LevelIDs
{
	LevelIDs_BasicTest,
	LevelIDs_KnucklesTest,
	LevelIDs_SonicTest,
	LevelIDs_GreenForest,
	LevelIDs_WhiteJungle,
	LevelIDs_PumpkinHill,
	LevelIDs_SkyRail,
	LevelIDs_AquaticMine,
	LevelIDs_SecurityHall,
	LevelIDs_PrisonLane,
	LevelIDs_MetalHarbor,
	LevelIDs_IronGate,
	LevelIDs_WeaponsBed,
	LevelIDs_CityEscape,
	LevelIDs_RadicalHighway,
	LevelIDs_WeaponsBed2P,
	LevelIDs_WildCanyon,
	LevelIDs_MissionStreet,
	LevelIDs_DryLagoon,
	LevelIDs_SonicVsShadow1,
	LevelIDs_TailsVsEggman1,
	LevelIDs_SandOcean,
	LevelIDs_CrazyGadget,
	LevelIDs_HiddenBase,
	LevelIDs_EternalEngine,
	LevelIDs_DeathChamber,
	LevelIDs_EggQuarters,
	LevelIDs_LostColony,
	LevelIDs_PyramidCave,
	LevelIDs_TailsVsEggman2,
	LevelIDs_FinalRush,
	LevelIDs_GreenHill,
	LevelIDs_MeteorHerd,
	LevelIDs_KnucklesVsRouge,
	LevelIDs_CannonsCoreS,
	LevelIDs_CannonsCoreE,
	LevelIDs_CannonsCoreT,
	LevelIDs_CannonsCoreR,
	LevelIDs_CannonsCoreK,
	LevelIDs_MissionStreet2P,
	LevelIDs_FinalChase,
	LevelIDs_WildCanyon2P,
	LevelIDs_SonicVsShadow2,
	LevelIDs_CosmicWall,
	LevelIDs_MadSpace,
	LevelIDs_SandOcean2P,
	LevelIDs_DryLagoon2P,
	LevelIDs_PyramidRace,
	LevelIDs_HiddenBase2P,
	LevelIDs_PoolQuest,
	LevelIDs_PlanetQuest,
	LevelIDs_DeckRace,
	LevelIDs_DowntownRace,
	LevelIDs_CosmicWall2P,
	LevelIDs_GrindRace,
	LevelIDs_LostColony2P,
	LevelIDs_EternalEngine2P,
	LevelIDs_MetalHarbor2P,
	LevelIDs_IronGate2P,
	LevelIDs_DeathChamber2P,
	LevelIDs_BigFoot,
	LevelIDs_HotShot,
	LevelIDs_FlyingDog,
	LevelIDs_KingBoomBoo,
	LevelIDs_EggGolemS,
	LevelIDs_Biolizard,
	LevelIDs_FinalHazard,
	LevelIDs_EggGolemE,
	LevelIDs_Route101280 = 70,
	LevelIDs_KartRace,
	LevelIDs_ChaoWorld = 90,
	LevelIDs_Invalid
};

enum {
    TYPE_NONE,
    TYPE_EGG,
    TYPE_CHILD,
    TYPE_DUMMY1,
    TYPE_DUMMY2,
    TYPE_N_NORMAL,
    TYPE_H_NORMAL,
    TYPE_D_NORMAL,
    TYPE_N_SWIM,
    TYPE_H_SWIM,
    TYPE_D_SWIM,
    TYPE_N_FLY,
    TYPE_H_FLY,
    TYPE_D_FLY,
    TYPE_N_RUN,
    TYPE_H_RUN,
    TYPE_D_RUN,
    TYPE_N_POWER,
    TYPE_H_POWER,
    TYPE_D_POWER,
    TYPE_N_CHAOS,
    TYPE_H_CHAOS,
    TYPE_D_CHAOS,
    TYPE_TAILS,
    TYPE_KNUCKLES,
    TYPE_AMY,
    TYPE_MINIMAL,
    TYPE_OMOCHAO,
    TYPE_EGG_FOOT,
};

enum SADXBodyType : __int8
{
	SADXBodyType_Normal = 0x0,
	SADXBodyType_EggChao = 0x1,
	SADXBodyType_Omochao = 0x2,
	SADXBodyType_None = 0x3,
};

enum SA2BAnimal : __int8
{
	SA2BAnimal_None = -1,
	SA2BAnimal_Penguin = 0x0,
	SA2BAnimal_Seal = 0x1,
	SA2BAnimal_Otter = 0x2,
	SA2BAnimal_Rabbit = 0x3,
	SA2BAnimal_Cheetah = 0x4,
	SA2BAnimal_Warthog = 0x5,
	SA2BAnimal_Bear = 0x6,
	SA2BAnimal_Tiger = 0x7,
	SA2BAnimal_Gorilla = 0x8,
	SA2BAnimal_Peacock = 0x9,
	SA2BAnimal_Parrot = 0xA,
	SA2BAnimal_Condor = 0xB,
	SA2BAnimal_Skunk = 0xC,
	SA2BAnimal_Sheep = 0xD,
	SA2BAnimal_Raccoon = 0xE,
	SA2BAnimal_HalfFish = 0xF,
	SA2BAnimal_SkeletonDog = 0x10,
	SA2BAnimal_Bat = 0x11,
	SA2BAnimal_Dragon = 0x12,
	SA2BAnimal_Unicorn = 0x13,
	SA2BAnimal_Phoenix = 0x14,
	SA2BAnimal_YellowChaosDrive = 0x15,
	SA2BAnimal_GreenChaosDrive = 0x16,
	SA2BAnimal_RedChaosDrive = 0x17,
	SA2BAnimal_PurpleChaosDrive = 0x18,
};

enum {
	IM_MWK = 0x1,
	IM_TWK = 0x2,
	IM_FWK = 0x4,
	IM_AWK = 0x8,
};

enum ChaoItemCategory : __int8
{
	ChaoItemCategory_Egg = 0x1,
	ChaoItemCategory_Fruit = 0x3,
	ChaoItemCategory_Seed = 0x7,
	ChaoItemCategory_Accessory = 0x8, //cwe
	ChaoItemCategory_Hat = 0x9,
	ChaoItemCategory_Special = 0xA, //cwe
	ChaoItemCategory_TransporterUnlock, //cwe
	ChaoItemCategory_MenuTheme = 0x10,
	ChaoItemCategory_Count
};

enum PDD_DEV_SUPPORT
{
	PDD_DEV_SUPPORT_AY2 = 0x200000,
	PDD_DEV_SUPPORT_AX2 = 0x100000,
	PDD_DEV_SUPPORT_AY1 = 0x80000,
	PDD_DEV_SUPPORT_AX1 = 0x40000,
	PDD_DEV_SUPPORT_AL = 0x20000,
	PDD_DEV_SUPPORT_AR = 0x10000,
	PDD_DEV_SUPPORT_KRB = 0x8000,
	PDD_DEV_SUPPORT_KLB = 0x4000,
	PDD_DEV_SUPPORT_KDB = 0x2000,
	PDD_DEV_SUPPORT_KUB = 0x1000,
	PDD_DEV_SUPPORT_TD = 0x800,
	PDD_DEV_SUPPORT_TX = 0x400,
	PDD_DEV_SUPPORT_TY = 0x200,
	PDD_DEV_SUPPORT_TZ = 0x100,
	PDD_DEV_SUPPORT_KR = 0x80,
	PDD_DEV_SUPPORT_KL = 0x40,
	PDD_DEV_SUPPORT_KD = 0x20,
	PDD_DEV_SUPPORT_KU = 0x10,
	PDD_DEV_SUPPORT_ST = 0x8,
	PDD_DEV_SUPPORT_TA = 0x4,
	PDD_DEV_SUPPORT_TB = 0x2,
	PDD_DEV_SUPPORT_TC = 0x1,
};

enum PDD_DGT
{
	PDD_DGT_TL = 0x20000,
	PDD_DGT_TR = 0x10000,
	PDD_DGT_KRB = 0x8000,
	PDD_DGT_KLB = 0x4000,
	PDD_DGT_KDB = 0x2000,
	PDD_DGT_KUB = 0x1000,
	PDD_DGT_TD = 0x800,
	PDD_DGT_TX = 0x400,
	PDD_DGT_TY = 0x200,
	PDD_DGT_TZ = 0x100,
	PDD_DGT_KR = 0x80,
	PDD_DGT_KL = 0x40,
	PDD_DGT_KD = 0x20,
	PDD_DGT_KU = 0x10,
	PDD_DGT_ST = 0x8,
	PDD_DGT_TA = 0x4,
	PDD_DGT_TB = 0x2,
	PDD_DGT_TC = 0x1,
};

enum PDD_DEVTYPE
{
	PDD_DEVTYPE_CONTROLLER = 0x1,
	PDD_DEVTYPE_STORAGE = 0x2,
	PDD_DEVTYPE_LCD = 0x4,
	PDD_DEVTYPE_TIMER = 0x8,
	PDD_DEVTYPE_SOUNDINPUT = 0x10,
	PDD_DEVTYPE_KEYBOARD = 0x40,
	PDD_DEVTYPE_LIGHTGUN = 0x80,
	PDD_DEVTYPE_VIBRATION = 0x100,
};

enum PDD_DEVAREA
{
	PDD_DEVAREA_USA = 0x1,
	PDD_DEVAREA_JAPAN = 0x2,
	PDD_DEVAREA_ASIA = 0x4,
	PDD_DEVAREA_EUROPE = 0x8,
	PDD_DEVAREA_RESERVED1 = 0x10,
	PDD_DEVAREA_RESERVED2 = 0x20,
	PDD_DEVAREA_RESERVED3 = 0x40,
	PDD_DEVAREA_RESERVED4 = 0x80,
};

enum PDD_CONDIR
{
	PDD_CONDIR_TOPSIDE = 0x0,
	PDD_CONDIR_BOTTOMSIDE = 0x1,
	PDD_CONDIR_LEFTSIDE = 0x2,
	PDD_CONDIR_RIGHTSIDE = 0x3,
};

#define BTN_L               PDD_DGT_TL
#define BTN_R               PDD_DGT_TR
#define BTN_X               PDD_DGT_TX
#define BTN_Y               PDD_DGT_TY
#define BTN_RIGHT           PDD_DGT_KR
#define BTN_LEFT            PDD_DGT_KL
#define BTN_DOWN            PDD_DGT_KD
#define BTN_UP              PDD_DGT_KU
#define BTN_START           PDD_DGT_ST
#define BTN_A               PDD_DGT_TA
#define BTN_B               PDD_DGT_TB

/****** Extended Buttons ************************************************************/
#define BTN_ZL              PDD_DGT_TC
#define BTN_ZR              PDD_DGT_TZ
#define BTN_BACK            PDD_DGT_TD

enum /* ssGameMode */
{
    MD_GAME_NONE,                   /* not currently in-game (eg. in menu or event) */
    MD_GAME_INIT,                   /* stage loading                                */
    MD_GAME_FADEIN = 0x07,          /* screen fading in                             */
    MD_GAME_FADEOUT_CLEAR = 0x08,   /* screen fading out after clearing stage       */
    MD_GAME_FADEOUT_MISS,
    MD_GAME_FADEOUT_RESTART = 0x0D, /* screen fading out for restart                */
    MD_GAME_MAIN  = 0x10,           /* game playing as normal                       */
    MD_GAME_PAUSE = 0x11,           /* game paused                                  */
    NUM_GAMEMD = 0x32,
};

enum StatusBits
{
	StatusBits_Ground,
	StatusBits_Unknown1,
	StatusBits_Hurt,
	StatusBits_ObjectInteract,
	StatusBits_Unknown2,
	StatusBits_Unknown3,
	StatusBits_Unknown4,
	StatusBits_Unknown5,
	StatusBits_Ball,
	StatusBits_LightDash,
	StatusBits_Attack,
	StatusBits_HoldObject,
	StatusBits_DoNextAction,
	StatusBits_OnPath,
	StatusBits_DisableControl,
	StatusBits_Unknown6
};

enum Status : uint16_t
{
	makemask(Status, Ground),
	makemask(Status, Unknown1),
	makemask(Status, Hurt),
	makemask(Status, ObjectInteract),
	makemask(Status, Unknown2),
	makemask(Status, Unknown3),
	makemask(Status, Unknown4),
	makemask(Status, Unknown5),
	makemask(Status, Ball),
	makemask(Status, LightDash),
	makemask(Status, Attack),
	makemask(Status, HoldObject),
	makemask(Status, DoNextAction),
	makemask(Status, OnPath),
	makemask(Status, DisableControl),
	makemask(Status, Unknown6)
};

enum SpecialTier
{
	SpecialTier_None = 0x0,
	SpecialTier_1 = 0x1,
	SpecialTier_2 = 0x2,
	SpecialTier_3 = 0x4,
};
