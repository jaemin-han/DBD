// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeZone.h"

#include "Components/BoxComponent.h"
#include "GameMode/DBDGameState.h"
#include "GameMode/DBDPlayerController.h"


// Sets default values
AEscapeZone::AEscapeZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	// set collision preset
	BoxComp->SetCollisionProfileName(FName("OverlapAll"));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEscapeZone::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AEscapeZone::OnComponentBeginOverlap);
	GameState = GetWorld()->GetGameState<ADBDGameState>();
}

void AEscapeZone::Interaction(APawn* Caller)
{
}

void AEscapeZone::FailedInteraction()
{
}

void AEscapeZone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                          bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버에서만 처리
	if (!HasAuthority())
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("AEscapeZone::OnComponentBeginOverlap"));
	// overlap 한 엑터가 Survivor 이름을 포함하는지 확인
	if (OtherActor->GetName().Contains(TEXT("Survivor")))
	{
		// todo: 생존자 탈출 처리
		GameState->SetSurvivorCount(GameState->GetSurvivorCount() - 1);

		// 남은 생존자가 0명이라면 게임 오버
		if (GameState->GetSurvivorCount() == 0)
		{
			GameState->MultiRPC_GameOver();
		}
		else
		{
			// 생존자 엑터를 spector로 변경
			if (auto* pawn = Cast<APawn>(OtherActor))
			{
				if (auto* pc = Cast<ADBDPlayerController>(pawn->GetController()))
				{
					pc->ServerRPC_ChangeToSpector();
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to cast to ADBDPlayerController"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to cast to APawn"));
			}
		}
	}
}
