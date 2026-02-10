#include "stdafx.h"
#include <AL_ModAPI.h>
#include <ChaoMain.h>
#include <data/minimal/minimal_all.h>

void ALAPI_RegisterMinimal(CWE_REGAPI_LEGACY* cwe_api) {
	bee_entry.Name = "Bee";
	ladybird_entry.Name = "Ladybird";
	scorpion_entry.Name = "Scorpion";

	BeeID = cwe_api->AddChaoMinimal(&bee_entry);
	cwe_api->AddChaoMinimal(&ladybird_entry);
	cwe_api->AddChaoMinimal(&scorpion_entry);

	cwe_api->RegisterChaoMinimalFruit(OrangeID, BeeID, 0, 40);
	cwe_api->RegisterChaoMinimalFruit(OrangeID, BeeID + 1, 40, 80);
	cwe_api->RegisterChaoMinimalFruit(OrangeID, BeeID + 2, 80, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_OrangeFruit, 25 + SADXAnimal_Otter, 0, 35);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_OrangeFruit, 25 + SADXAnimal_Seal, 35, 50);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_OrangeFruit, 25 + SADXAnimal_Lion, 50, 75);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_OrangeFruit, 25 + SADXAnimal_Gorilla, 75, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_BlueFruit, 25 + SADXAnimal_Swallow, 0, 40);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_BlueFruit, 25 + SADXAnimal_Peacock, 40, 60);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_BlueFruit, 25 + SADXAnimal_Parrot, 60, 80);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_BlueFruit, 25 + SADXAnimal_Penguin, 80, 90);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_BlueFruit, 25 + SADXAnimal_Koala, 90, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PinkFruit, 25 + SADXAnimal_Otter, 0, 15);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PinkFruit, 25 + SADXAnimal_Seal, 15, 50);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PinkFruit, 25 + SADXAnimal_Kangaroo, 50, 90);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PinkFruit, 25 + SADXAnimal_Mole, 90, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_GreenFruit, 25 + SADXAnimal_Elephant, 0, 40);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_GreenFruit, 25 + SADXAnimal_Deer, 40, 70);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_GreenFruit, 25 + SADXAnimal_Kangaroo, 70, 90);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_GreenFruit, 25 + SADXAnimal_Mole, 90, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PurpleFruit, 25 + SADXAnimal_Rabbit, 0, 30);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PurpleFruit, 25 + SADXAnimal_Peacock, 30, 50);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PurpleFruit, 25 + SADXAnimal_Koala, 50, 60);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PurpleFruit, 25 + SADXAnimal_Mole, 60, 70);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_PurpleFruit, 25 + SADXAnimal_Skunk, 70, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_YellowFruit, 25 + SADXAnimal_Swallow, 0, 30);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_YellowFruit, 25 + SADXAnimal_Parrot, 30, 70);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_YellowFruit, 25 + SADXAnimal_Gorilla, 70, 80);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_YellowFruit, 25 + SADXAnimal_Koala, 80, 90);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_YellowFruit, 25 + SADXAnimal_Skunk, 90, 100);

	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_RedFruit, 25 + SADXAnimal_Penguin, 0, 20);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_RedFruit, 25 + SADXAnimal_Lion, 20, 50);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_RedFruit, 25 + SADXAnimal_Deer, 50, 70);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_RedFruit, 25 + SADXAnimal_Koala, 70, 80);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_RedFruit, 25 + SADXAnimal_Mole, 80, 90);
	cwe_api->RegisterChaoMinimalFruit(SA2BFruit_RedFruit, 25 + SADXAnimal_Skunk, 90, 100);

}