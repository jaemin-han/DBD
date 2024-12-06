// Fill out your copyright notice in the Description page of Project Settings.


#include "DBDPlayerController.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpectatorPawn.h"

void ADBDPlayerController::ServerRPC_ChangeToSpector_Implementation()
{
	auto* CurPlayer = GetPawn();

	UnPossess();

	auto* GameMode = GetWorld()->GetAuthGameMode();
	auto* Spector = GetWorld()->SpawnActor<ASpectatorPawn>(GameMode->SpectatorClass, CurPlayer->GetActorTransform());

	Possess(Spector);

	CurPlayer->Destroy();
}
