// Fill out your copyright notice in the Description page of Project Settings.


#include "DBDGameState.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"

ADBDGameState::ADBDGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ADBDGameState::BeginPlay()
{
	Super::BeginPlay();

	PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>();
	PostProcessVolume->bUnbound = true;
	AddDynamicMaterialToPostProcessVolume();
}


void ADBDGameState::AddDynamicMaterialToPostProcessVolume()
{
	if (PostProcessVolume && MaterialInstance)
	{
		PostProcessVolume->AddOrUpdateBlendable(MaterialInstance);
	}
}
