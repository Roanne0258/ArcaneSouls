// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArcaneSoulsGameMode.h"
#include "ArcaneSoulsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AArcaneSoulsGameMode::AArcaneSoulsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
