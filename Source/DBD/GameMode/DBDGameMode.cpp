// Copyright Epic Games, Inc. All Rights Reserved.

#include "DBDGameMode.h"
#include "../Character/DBDCharacter.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "DBDGameInstance.h"

ADBDGameMode::ADBDGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ADBDGameMode::BeginPlay()
{
	Super::BeginPlay();
}