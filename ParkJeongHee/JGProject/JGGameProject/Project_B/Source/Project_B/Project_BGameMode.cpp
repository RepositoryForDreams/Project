// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_BGameMode.h"
#include "Project_BCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProject_BGameMode::AProject_BGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
