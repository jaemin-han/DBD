// Copyright Epic Games, Inc. All Rights Reserved.

#include "DBDGameMode.h"
#include "../Character/DBDCharacter.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


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

void ADBDGameMode::ChangeCharacter()
{
	// controller
	auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// find current possessed character
	auto* PossessedCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// PossessedCharacter 가 killer 라면
	if (Cast<AKiller>(PossessedCharacter))
	{
		// find survivor instance
		auto* PossessActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADBD_Player::StaticClass());
		auto PossessSurvivor = Cast<ADBD_Player>(PossessActor);
		PlayerController->Possess(PossessSurvivor);
	}
	else
	{
		// find killer instance
		auto* PossessActor = UGameplayStatics::GetActorOfClass(GetWorld(), AKiller::StaticClass());
		auto PossessKiller = Cast<AKiller>(PossessActor);
		PlayerController->Possess(PossessKiller);
	}
}
