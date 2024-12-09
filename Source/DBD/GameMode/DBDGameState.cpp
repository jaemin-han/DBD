// Fill out your copyright notice in the Description page of Project Settings.


#include "DBDGameState.h"

#include "DBDGameInstance.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Gimmick/Generator.h"
#include "Gimmick/Hanger.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

void ADBDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADBDGameState, SurvivorCount);
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

void ADBDGameState::SetCustomDepthSurvivors(ADBD_Player* CurrentPlayer, bool bIsVisible)
{
	// 로컬 플레이어 인 경우에만 실행
	if (CurrentPlayer->IsLocallyControlled())
	{
		for (APlayerState* PlayerState : PlayerArray)
		{
			ADBD_Player* Survivor = Cast<ADBD_Player>(PlayerState->GetPawn());
			if (Survivor && Survivor != CurrentPlayer)
			{
				Survivor->GetMesh()->SetRenderCustomDepth(bIsVisible);
			}
		}
	}
}

void ADBDGameState::SetCustomDepthOnThisSurvivor_Implementation(class ADBD_Player* CurrentPlayer, bool bIsVisible)
{
	// 로컬 플레이어가 아닌 경우에만 실행 , multiRPC 라서 CurrentPlayer 가 설정되기 전에 실행되는 경우 방지
	if (CurrentPlayer && !CurrentPlayer->IsLocallyControlled())
	{
		// 첫 번째 local player controller 가 killer 인 경우 실행하지 않음
		auto* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController->GetPawn()->IsA(AKiller::StaticClass()))
		{
			return;
		}
		
		CurrentPlayer->GetMesh()->SetRenderCustomDepth(bIsVisible);
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

void ADBDGameState::MultiRPC_GameOver_Implementation()
{
	// game state 의 여러 상태에 따라서 게임 종료 처리
	FString netrole = UEnum::GetValueAsString(GetLocalRole());
	UE_LOG(LogTemp, Error, TEXT("%s: Game Over"), *netrole);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		if (HasAuthority())
		{
			GetWorld()->ServerTravel(TEXT("/Game/DBD/Level/LobbyLevel?listen"));
		}
		
		// auto* GameInstance = Cast<UDBDGameInstance>(GetWorld()->GetGameInstance());
		// if (GameInstance)
		// {
		// 	GameInstance->DestroySession(GameInstance->SName);
		// }
		
	}), 3.0f, false);
}
