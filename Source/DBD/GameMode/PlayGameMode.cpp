// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGameMode.h"

#include "Character/DBD_Player.h"
#include "Character/Killer.h"

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
