// Fill out your copyright notice in the Description page of Project Settings.


#include "DBDGameState.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Gimmick/Generator.h"
#include "Gimmick/Hanger.h"
#include "Kismet/GameplayStatics.h"

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

void ADBDGameState::SetGeneratorCustomDepth(bool bIsVisible)
{
	for (auto* Generator : Generators)
	{
		Generator->GetMeshComp()->SetRenderCustomDepth(bIsVisible);
	}
}

void ADBDGameState::SetHangerCustomDepth(bool bIsVisible)
{
	for (auto* Hanger : Hangers)
	{
		Hanger->GetMeshComp()->SetRenderCustomDepth(bIsVisible);
	}
}

void ADBDGameState::InitArrays()
{
	// generators 배열 초기화

	TArray<AActor*> Actors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGenerator::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		Generators.Add(Cast<AGenerator>(Actor));
	}
	Actors.Empty();
	// Hangers 배열 초기화
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHanger::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		Hangers.Add(Cast<AHanger>(Actor));
	}
}
