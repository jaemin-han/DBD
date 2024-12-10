// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGameMode.h"

#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "DBDGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"


void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();
	index = 1;
}

void APlayGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	index = 1;
}

APawn* APlayGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnDefaultPawnFor_Implementation"));
	if (NewPlayer->IsLocalController())
	{
		return GetWorld()->SpawnActor<AKiller>(KillerClass, StartSpot->GetActorLocation(),
		                                       StartSpot->GetActorRotation());
	}
	else
	{
		return GetWorld()->SpawnActor<ADBD_Player>(SurvivorClass, StartSpot->GetActorLocation(),
		                                           StartSpot->GetActorRotation());
	}
}

AActor* APlayGameMode::ChoosePlayerStart_Implementation(AController* Player)
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
