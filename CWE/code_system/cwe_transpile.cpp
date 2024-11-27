#include "stdafx.h"
#include "..\SA2ModLoader.h"
#include <al_save.h>
#include <ChaoMain.h>
#define CHAO_BASE (0x019F6EC4)
#define CHAO_SLOT(a) (CHAO_BASE + a * 0x800)
int Regs[16];

void ChaoWorldExtendedRequired()
{
	//fix this later to not be 24 for future proofing (function from symbols that "return 24")
	if (CurrentLevel == 90) { //this line was added by me, i dont see a reason for all these checks to run otherwise
		for (int i = 0; i < 24; i++) {
			//REMOVED: shiny fruit value for FCE hack (FCE is not very common anymore)

			//x rank
			//todo clean this up
			if (*(uint8_t*)(CHAO_SLOT(i) + 0x59e) == 0)
			{
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x28) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x28) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x29) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x29) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2a) == 6)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2a) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2b) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2b) = 5;
				}
			}
			else if (*(uint8_t*)(CHAO_SLOT(i) + 0x59e) == 1)
			{
				*(uint8_t*)(CHAO_SLOT(i) + 0x28) = 7;
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x29) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x29) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2a) == 6)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2a) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2b) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2b) = 5;
				}
			}
			else if (*(uint8_t*)(CHAO_SLOT(i) + 0x59e) == 2)
			{
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x28) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x28) = 5;
				}
				*(uint8_t*)(CHAO_SLOT(i) + 0x29) = 7;
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2a) == 6)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2a) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2b) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2b) = 5;
				}
			}
			else if (*(uint8_t*)(CHAO_SLOT(i) + 0x59e) == 3)
			{
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x28) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x28) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x29) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x29) = 5;
				}
				*(uint8_t*)(CHAO_SLOT(i) + 0x2a) = 6;
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2b) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2b) = 5;
				}
			}
			else if (*(uint8_t*)(CHAO_SLOT(i) + 0x59e) == 4)
			{
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x28) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x28) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x29) == 7)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x29) = 5;
				}
				if (*(uint8_t*)(CHAO_SLOT(i) + 0x2a) == 6)
				{
					*(uint8_t*)(CHAO_SLOT(i) + 0x2a) = 5;
				}
				*(uint8_t*)(CHAO_SLOT(i) + 0x2b) = 7;
			}

			//x rank reincarnation stat cap
			if (ChaoSlots[i].data.Type == 1) //if chao is egg
			{
				ChaoSlots[i].data.XGradeValue = 0;

				int cap;
				switch (ChaoSlots[i].data.Reincarnations) {
				case 0:
					cap = 0;
					break;
				case 1:
					cap = 297;
					break;
				case 2:
					cap = 323;
					break;
				default: // >= 3
					cap = 326;
					break;
				}

				if (cap > 0) {
					for (int j = 0; j < 4; j++) {
						if (ChaoSlots[i].data.StatPoints[j] > cap)
							ChaoSlots[i].data.StatPoints[j] = cap;
					}
				}
			}

			//cap emotions (im guessing emotions that get edited by lst)
			if (ChaoSlots[i].data.Emotion.Dizziness > 200)
			{
				ChaoSlots[i].data.Emotion.Dizziness = 200;
			}
			if (ChaoSlots[i].data.Emotion.Surprise > 200)
			{
				ChaoSlots[i].data.Emotion.Surprise = 200;
			}
			if (ChaoSlots[i].data.Emotion.UrgeToCry > 200)
			{
				ChaoSlots[i].data.Emotion.UrgeToCry = 200;
			}
			if (ChaoSlots[i].data.Emotion.Anger > 200)
			{
				ChaoSlots[i].data.Emotion.Anger = 200;
			}

			//swim points
			if (gConfigVal.EnergyCap && ChaoSlots[i].data.StatPoints[0] <= 99)
			{
				//cap energy at 5000 if the chao cant swim yet
				if (ChaoSlots[i].data.Emotion.Energy > 5000)
				{
					ChaoSlots[i].data.Emotion.Energy = 5000;
				}
			}
		}
	}

	//hero garden spawnpoint moved
	if (*(float*)0x013663D4 == -3.5f)
	{
		if (*(float*)0x013663D8 == 0)
		{
			if (*(float*)0x013663DC == -85)
			{
				*(float*)0x013663D4 = -35;
				*(float*)0x013663D8 = 12;
				*(float*)0x013663DC = -128;
			}
		}
	}

	//no clue
	if (*(uint32_t*)0x01A529EC == 0x1860)
	{
		if (*(uint8_t*)0x01934be0 == 16)
		{
			*(uint8_t*)0x01934be0 = 14;
		}
		if (*(uint8_t*)0x01934be0 == 17)
		{
			*(uint8_t*)0x01934be0 = 14;
		}
	}

	//transporter colors
	if (gConfigVal.TransporterColor) {
		if (*(uint32_t*)0x0134062c == 1)
		{
			*(uint32_t*)0x0125715C = 0xFFFFEF50;
			*(uint32_t*)0x01257D1C = 0xFF85E3FF;
			*(uint32_t*)0x012567c4 = 0xFFFFEF50;
			*(uint32_t*)0x01256424 = 0xFFFF0000;
			*(uint32_t*)0x01257E5C = 0xFFFF0000;
			*(uint8_t*)0x008B71C5 = 0x31;
		}
		else if (*(uint32_t*)0x0134062c == 2)
		{
			*(uint32_t*)0x0125715C = 0xFF50C0FF;
			*(uint32_t*)0x01257D1C = 0xFFFFFFFF;
			*(uint32_t*)0x012567c4 = 0xFF50C0FF;
			*(uint32_t*)0x01256424 = 0xFFFF0000;
			*(uint32_t*)0x01257E5C = 0xFFA0FF80;
			*(uint8_t*)0x008B71C5 = 0x32;
		}
		else if (*(uint32_t*)0x0134062c == 3)
		{
			*(uint32_t*)0x0125715C = 0xFFffa0a0;
			*(uint32_t*)0x01257D1C = 0xFFa080a0;
			*(uint32_t*)0x012567c4 = 0xFFffa0a0;
			*(uint32_t*)0x01256424 = 0xFFFF0000;
			*(uint32_t*)0x01257E5C = 0xFFFF80FF;
			*(uint8_t*)0x008B71C5 = 0x33;
		}
	}
	//more inventory safe guard crap
	bool saveFull = true;
	for (int i = 0; i < 24; i++) {
		if (ChaoSlots[i].data.Type == 0)
			saveFull = false;
	}
	if (saveFull) {
		if (cweSaveFile.purchasedItemCount == 1 && save::CWE_PurchasedItems[0].mCategory == 1) {
			cweSaveFile.purchasedItemCount = 0;
			save::CWE_PurchasedItems[0] = { 0,0 };
		}
	}

	//ncr crap
	if (*(uint32_t*)0x0134062c == 3)
	{
		if (*(uint16_t*)0x012D9914 == 0x4c41)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3130)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3230)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3330)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3430)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3530)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3630)
		{
			if (*(uint8_t*)0x0174AFDD == 48)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 36)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 12)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3730)
		{
			if (*(uint8_t*)0x0174AFDD == 50)
			{
				*(uint16_t*)0x0125bff2 = 12594;
				*(uint16_t*)0x0125C09A = 12594;
				*(uint16_t*)0x0125C00a = 12850;
				*(uint16_t*)0x0125C052 = 12850;
				*(uint16_t*)0x0125C022 = 13106;
				*(uint16_t*)0x0125C06A = 13106;
				*(uint16_t*)0x0125C03A = 13362;
				*(uint16_t*)0x0125C082 = 13362;
			}
			if (*(uint8_t*)0x0174AFDD == 40)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 30)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 20)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 10)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3830)
		{
			if (*(uint8_t*)0x0174AFDD == 50)
			{
				*(uint16_t*)0x0125bff2 = 13618;
				*(uint16_t*)0x0125C09A = 13618;
				*(uint16_t*)0x0125C00a = 13874;
				*(uint16_t*)0x0125C052 = 13874;
				*(uint16_t*)0x0125C022 = 14130;
				*(uint16_t*)0x0125C06A = 14130;
				*(uint16_t*)0x0125C03A = 14386;
				*(uint16_t*)0x0125C082 = 14386;
			}
			if (*(uint8_t*)0x0174AFDD == 40)
			{
				*(uint16_t*)0x0125bff2 = 12594;
				*(uint16_t*)0x0125C09A = 12594;
				*(uint16_t*)0x0125C00a = 12850;
				*(uint16_t*)0x0125C052 = 12850;
				*(uint16_t*)0x0125C022 = 13106;
				*(uint16_t*)0x0125C06A = 13106;
				*(uint16_t*)0x0125C03A = 13362;
				*(uint16_t*)0x0125C082 = 13362;
			}
			if (*(uint8_t*)0x0174AFDD == 32)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 16)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 8)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3930)
		{
			if (*(uint8_t*)0x0174AFDD == 50)
			{
				*(uint16_t*)0x0125bff2 = 13618;
				*(uint16_t*)0x0125C09A = 13618;
				*(uint16_t*)0x0125C00a = 13874;
				*(uint16_t*)0x0125C052 = 13874;
				*(uint16_t*)0x0125C022 = 14130;
				*(uint16_t*)0x0125C06A = 14130;
				*(uint16_t*)0x0125C03A = 14386;
				*(uint16_t*)0x0125C082 = 14386;
			}
			if (*(uint8_t*)0x0174AFDD == 40)
			{
				*(uint16_t*)0x0125bff2 = 12594;
				*(uint16_t*)0x0125C09A = 12594;
				*(uint16_t*)0x0125C00a = 12850;
				*(uint16_t*)0x0125C052 = 12850;
				*(uint16_t*)0x0125C022 = 13106;
				*(uint16_t*)0x0125C06A = 13106;
				*(uint16_t*)0x0125C03A = 13362;
				*(uint16_t*)0x0125C082 = 13362;
			}
			if (*(uint8_t*)0x0174AFDD == 32)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 16)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 8)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3031)
		{
			if (*(uint8_t*)0x0174AFDD == 50)
			{
				*(uint16_t*)0x0125bff2 = 13618;
				*(uint16_t*)0x0125C09A = 13618;
				*(uint16_t*)0x0125C00a = 13874;
				*(uint16_t*)0x0125C052 = 13874;
				*(uint16_t*)0x0125C022 = 14130;
				*(uint16_t*)0x0125C06A = 14130;
				*(uint16_t*)0x0125C03A = 14386;
				*(uint16_t*)0x0125C082 = 14386;
			}
			if (*(uint8_t*)0x0174AFDD == 40)
			{
				*(uint16_t*)0x0125bff2 = 12594;
				*(uint16_t*)0x0125C09A = 12594;
				*(uint16_t*)0x0125C00a = 12850;
				*(uint16_t*)0x0125C052 = 12850;
				*(uint16_t*)0x0125C022 = 13106;
				*(uint16_t*)0x0125C06A = 13106;
				*(uint16_t*)0x0125C03A = 13362;
				*(uint16_t*)0x0125C082 = 13362;
			}
			if (*(uint8_t*)0x0174AFDD == 32)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 16)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 8)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3131)
		{
			if (*(uint8_t*)0x0174AFDD == 50)
			{
				*(uint16_t*)0x0125bff2 = 13618;
				*(uint16_t*)0x0125C09A = 13618;
				*(uint16_t*)0x0125C00a = 13874;
				*(uint16_t*)0x0125C052 = 13874;
				*(uint16_t*)0x0125C022 = 14130;
				*(uint16_t*)0x0125C06A = 14130;
				*(uint16_t*)0x0125C03A = 14386;
				*(uint16_t*)0x0125C082 = 14386;
			}
			if (*(uint8_t*)0x0174AFDD == 40)
			{
				*(uint16_t*)0x0125bff2 = 12594;
				*(uint16_t*)0x0125C09A = 12594;
				*(uint16_t*)0x0125C00a = 12850;
				*(uint16_t*)0x0125C052 = 12850;
				*(uint16_t*)0x0125C022 = 13106;
				*(uint16_t*)0x0125C06A = 13106;
				*(uint16_t*)0x0125C03A = 13362;
				*(uint16_t*)0x0125C082 = 13362;
			}
			if (*(uint8_t*)0x0174AFDD == 32)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 16)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 8)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
		if (*(uint16_t*)0x012D9914 == 0x3231)
		{
			if (*(uint8_t*)0x0174AFDD == 50)
			{
				*(uint16_t*)0x0125bff2 = 13618;
				*(uint16_t*)0x0125C09A = 13618;
				*(uint16_t*)0x0125C00a = 13874;
				*(uint16_t*)0x0125C052 = 13874;
				*(uint16_t*)0x0125C022 = 14130;
				*(uint16_t*)0x0125C06A = 14130;
				*(uint16_t*)0x0125C03A = 14386;
				*(uint16_t*)0x0125C082 = 14386;
			}
			if (*(uint8_t*)0x0174AFDD == 40)
			{
				*(uint16_t*)0x0125bff2 = 12594;
				*(uint16_t*)0x0125C09A = 12594;
				*(uint16_t*)0x0125C00a = 12850;
				*(uint16_t*)0x0125C052 = 12850;
				*(uint16_t*)0x0125C022 = 13106;
				*(uint16_t*)0x0125C06A = 13106;
				*(uint16_t*)0x0125C03A = 13362;
				*(uint16_t*)0x0125C082 = 13362;
			}
			if (*(uint8_t*)0x0174AFDD == 32)
			{
				*(uint16_t*)0x0125bff2 = 14129;
				*(uint16_t*)0x0125C09A = 14129;
				*(uint16_t*)0x0125C00a = 14385;
				*(uint16_t*)0x0125C052 = 14385;
				*(uint16_t*)0x0125C022 = 14641;
				*(uint16_t*)0x0125C06A = 14641;
				*(uint16_t*)0x0125C03A = 12338;
				*(uint16_t*)0x0125C082 = 12338;
			}
			if (*(uint8_t*)0x0174AFDD == 24)
			{
				*(uint16_t*)0x0125bff2 = 13105;
				*(uint16_t*)0x0125C09A = 13105;
				*(uint16_t*)0x0125C00a = 13361;
				*(uint16_t*)0x0125C052 = 13361;
				*(uint16_t*)0x0125C022 = 13617;
				*(uint16_t*)0x0125C06A = 13617;
				*(uint16_t*)0x0125C03A = 13873;
				*(uint16_t*)0x0125C082 = 13873;
			}
			if (*(uint8_t*)0x0174AFDD == 16)
			{
				*(uint16_t*)0x0125bff2 = 14693;
				*(uint16_t*)0x0125C09A = 14693;
				*(uint16_t*)0x0125C00a = 12337;
				*(uint16_t*)0x0125C052 = 12337;
				*(uint16_t*)0x0125C022 = 12593;
				*(uint16_t*)0x0125C06A = 12593;
				*(uint16_t*)0x0125C03A = 12849;
				*(uint16_t*)0x0125C082 = 12849;
			}
			if (*(uint8_t*)0x0174AFDD == 8)
			{
				*(uint16_t*)0x0125bff2 = 13669;
				*(uint16_t*)0x0125C09A = 13669;
				*(uint16_t*)0x0125C00a = 13925;
				*(uint16_t*)0x0125C052 = 13925;
				*(uint16_t*)0x0125C022 = 14181;
				*(uint16_t*)0x0125C06A = 14181;
				*(uint16_t*)0x0125C03A = 14437;
				*(uint16_t*)0x0125C082 = 14437;
			}
			if (*(uint8_t*)0x0174AFDD == 0)
			{
				*(uint16_t*)0x0125bff2 = 12645;
				*(uint16_t*)0x0125C09A = 12645;
				*(uint16_t*)0x0125C00a = 12901;
				*(uint16_t*)0x0125C052 = 12901;
				*(uint16_t*)0x0125C022 = 13157;
				*(uint16_t*)0x0125C06A = 13157;
				*(uint16_t*)0x0125C03A = 13413;
				*(uint16_t*)0x0125C082 = 13413;
			}
		}
	}

	//inventory safe guards for race codes that put crap in the inventory
	if (*(uint8_t*)0x01934be0 != 17)
	{
		if (*(uint8_t*)0x01DBEDAC > 6)
		{
			*(uint8_t*)0x01DBEDAC = 5;
		}
		if (*(uint8_t*)0x01DBEDAC == 5)
		{
			if (*(uint8_t*)0x01DBEDA8 == 255)
			{
				if (*(uint8_t*)0x01DBEDA6 == 255)
				{
					if (*(uint8_t*)0x01DBEDA4 == 255)
					{
						if (*(uint8_t*)0x01DBEDA2 == 255)
						{
							if (*(uint8_t*)0x01DBEDA0 == 255)
							{
								*(uint8_t*)0x01DBEDAC = 0;
							}
						}
					}
				}
			}
		}
		if (*(uint8_t*)0x01DBEDAC == 4)
		{
			if (*(uint8_t*)0x01DBEDA6 == 255)
			{
				if (*(uint8_t*)0x01DBEDA4 == 255)
				{
					if (*(uint8_t*)0x01DBEDA2 == 255)
					{
						if (*(uint8_t*)0x01DBEDA0 == 255)
						{
							*(uint8_t*)0x01DBEDAC = 0;
						}
					}
				}
			}
		}
		if (*(uint8_t*)0x01DBEDAC == 3)
		{
			if (*(uint8_t*)0x01DBEDA4 == 255)
			{
				if (*(uint8_t*)0x01DBEDA2 == 255)
				{
					if (*(uint8_t*)0x01DBEDA0 == 255)
					{
						*(uint8_t*)0x01DBEDAC = 0;
					}
				}
			}
		}
		if (*(uint8_t*)0x01DBEDAC == 2)
		{
			if (*(uint8_t*)0x01DBEDA2 == 255)
			{
				if (*(uint8_t*)0x01DBEDA0 == 255)
				{
					*(uint8_t*)0x01DBEDAC = 0;
				}
			}
		}
		if (*(uint8_t*)0x01DBEDAC == 1)
		{
			if (*(uint8_t*)0x01DBEDA0 == 255)
			{
				*(uint8_t*)0x01DBEDAC = 0;
			}
		}
		if (*(uint8_t*)0x01DBEDAC == 0)
		{
			*(uint8_t*)0x01DBEDA0 = 255;
			*(uint8_t*)0x01DBEDA1 = 0;
			*(uint8_t*)0x01DBEDA2 = 255;
			*(uint8_t*)0x01DBEDA3 = 0;
			*(uint8_t*)0x01DBEDA4 = 255;
			*(uint8_t*)0x01DBEDA5 = 0;
			*(uint8_t*)0x01DBEDA6 = 255;
			*(uint8_t*)0x01DBEDA7 = 0;
			*(uint8_t*)0x01DBEDA8 = 255;
			*(uint8_t*)0x01DBEDA9 = 0;
			*(uint8_t*)0x01DBEDAA = 0;
			*(uint8_t*)0x01DBEDAB = 0;
			*(uint8_t*)0x01DBEDAC = 0;
			*(uint8_t*)0x01DBEDAD = 0;
			*(uint8_t*)0x01DBEDAE = 0;
			*(uint8_t*)0x01DBEDAF = 0;
		}
	}

	//invincibility on maincharobj if chao world 
	if (CurrentLevel == 90 && MainCharObj2[0])
	{
		MainCharObj2[0]->Powerups = 1;
	}

	//REMOVED: that weird Y pos check thing
}