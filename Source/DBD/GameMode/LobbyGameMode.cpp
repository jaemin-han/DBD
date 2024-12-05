// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

#include "DBDGameInstance.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 화면에 PlayerCount 출력해주기
	
}

APawn* ALobbyGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	PlayerCount++;
	UE_LOG(LogTemp, Warning, TEXT("PlayerCount : %d"), PlayerCount);

	UE_LOG(LogTemp, Warning, TEXT("SpawnDefaultPawnFor_Implementation"));
	if (NewPlayer->IsLocalController())
	{
		return GetWorld()->SpawnActor<AKiller>(KillerClass, 
												ChoosePlayerStart(NewPlayer)->GetActorLocation(), 
												ChoosePlayerStart(NewPlayer)->GetActorRotation());
	}
	else
	{
		return GetWorld()->SpawnActor<ADBD_Player>(SurvivorClass, 
												ChoosePlayerStart(NewPlayer)->GetActorLocation(), 
												ChoosePlayerStart(NewPlayer)->GetActorRotation());
	}
}

AActor* ALobbyGameMode::ChoosePlayerStart(AController* Player)
{
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* playerStart = *It;

		if (playerStart->PlayerStartTag == "Killer" && Player->IsLocalController())
		{
			return playerStart;
		}
		else //if (playerStart->PlayerStartTag == "Survivor" && !Player->IsLocalController())
		{
			return Super::ChoosePlayerStart(Player);
		}
	}
	
	return Super::ChoosePlayerStart(Player);
}
