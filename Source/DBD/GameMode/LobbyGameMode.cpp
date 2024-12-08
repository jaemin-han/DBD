// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


#include "DBDGameInstance.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	index = 1;
}

void ALobbyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	PlayerCount = 0;
	index = 1;
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

	//StartSpot = ChoosePlayerStart(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("StartSpot Label : %s"), *StartSpot->GetActorLabel());

	if (NewPlayer->IsLocalController())
	{
		return GetWorld()->SpawnActor<AKiller>(KillerClass, 
			StartSpot->GetActorLocation(),
			StartSpot->GetActorRotation());
	}
	else
	{
		return GetWorld()->SpawnActor<ADBD_Player>(SurvivorClass, 
			StartSpot->GetActorLocation(),
			StartSpot->GetActorRotation());
	}
}

AActor* ALobbyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> foundPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), foundPlayerStarts);

	for (AActor* playerStart : foundPlayerStarts)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStart Label : %s"), *playerStart->GetActorLabel());
		UE_LOG(LogTemp, Warning, TEXT("index : %d"), index);
		if (Player->IsLocalController())
		{
			if (playerStart->GetActorLabel().Contains(TEXT("PlayerStart_Killer")))
			{
				return playerStart;
			}
		}
		else
		{
			FString tag = "PlayerStart_Survivor" + FString::FromInt(index);
			if (playerStart->GetActorLabel().Contains(tag))
			{
				index++;
				return playerStart;
			}
		}
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}
