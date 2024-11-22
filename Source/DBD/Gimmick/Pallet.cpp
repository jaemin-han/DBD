// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Pallet.h"

#include "Character/Killer.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
APallet::APallet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// wall comp
	WallComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WallComp"));
	WallComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallComp->SetCollisionObjectType(ECC_WorldStatic);
	WallComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	WallComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(WallComp);

	PalletMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PalletMeshComp"));
	PalletMeshComp->SetupAttachment(RootComponent);

	// Front, Back Indicator
	FrontIndicator = CreateDefaultSubobject<USphereComponent>(TEXT("FrontIndicator"));
	FrontIndicator->SetupAttachment(RootComponent);
	BackIndicator = CreateDefaultSubobject<USphereComponent>(TEXT("BackIndicator"));
	BackIndicator->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APallet::BeginPlay()
{
	Super::BeginPlay();

	CurrentRoll = GetActorRotation().Roll;
	TargetRoll = CurrentRoll + 60.0f;
}

// Called every frame
void APallet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFallen)
		return;

	if (!bIsInteracted)
		return;

	CurrentRoll += 180.0f * DeltaTime;
	PalletMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, CurrentRoll));

	if (TargetRoll - CurrentRoll < KINDA_SMALL_NUMBER)
	{
		bIsFallen = true;
		bIsInteracted = false;
		PalletMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, TargetRoll));
	}
}

void APallet::Interaction(AActor* Caller)
{
	// 상호작용 중이 아니고, 판자가 서 있으면 판자 내리기 수행
	if (bIsInteracted == false && bIsFallen == false)
	{
		PalletFall();
		return;
	}

	// 상호작용 중이 아니고, 판자가 넘어져 있고, 상호작용하는 엑터가 킬러라면 판자를 부셔요
	if (bIsInteracted == false && bIsFallen == true)
	{
		if (AKiller* Killer = Cast<AKiller>(Caller))
		{
			Killer->DestroyPallet();
		}
		else
		{
			// ue_log killer
			UE_LOG(LogTemp, Error, TEXT("APallet::Interaction - Failed to cast AKiller"));
		}
		return;
	}
}

void APallet::FailedInteraction()
{
}

void APallet::PalletFall()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Pallet Interaction"));
	bIsInteracted = true;

	// 판자를 내리는 공간에 캐틱터들이 존재하면 엑터들을 이동시킨 후, 물리 충돌 활성화
	TArray<AActor*> OverlappingActors;
	WallComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		float FrontDistance = FVector::Dist(FrontIndicator->GetComponentLocation(), Actor->GetActorLocation());
		float BackDistance = FVector::Dist(BackIndicator->GetComponentLocation(), Actor->GetActorLocation());

		if (FrontDistance < BackDistance)
			Actor->SetActorLocation(FrontIndicator->GetComponentLocation());
		else
			Actor->SetActorLocation(BackIndicator->GetComponentLocation());

		// 킬러가 판자에 맞으면, 기절 상태로 만든다.
		AKiller* Killer = Cast<AKiller>(Actor);
		if (Killer)
			Killer->Stun();
	}
	WallComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}
