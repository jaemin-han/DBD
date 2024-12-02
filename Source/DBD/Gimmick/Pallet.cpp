// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Pallet.h"

#include "Character/Killer.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
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

void APallet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APallet, bIsInteracted);
	DOREPLIFETIME(APallet, bIsFallen);
}

// Called every frame
void APallet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// debug
	// DebugOwner();

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

		// WallComp 의 Visibility 와의 충돌 판정을 block 으로 설정
		// 이걸 해야 판자가 내려간 다음 킬러가 line trace 를 활용해 판자를 부술 수 있음
		WallComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void APallet::Interaction(AActor* Caller)
{
	// 상호작용 중이 아니고, 판자가 서 있으면 판자 내리기 수행
	if (bIsInteracted == false && bIsFallen == false && not Cast<AKiller>(Caller))
	{
		ServerRPC_PalletFall();
		return;
	}
	// 상호작용 중이 아니고, 판자가 넘어져 있고, 상호작용하는 엑터가 킬러라면 판자를 부셔요
	if (bIsInteracted == false && bIsFallen == true && Cast<AKiller>(Caller))
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
	}
}

void APallet::FailedInteraction()
{
}

FString APallet::GetGimmickName()
{
	return Name;
}

FString APallet::GetInteractKey()
{
	return InteractKey;
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
		{
			// Actor->SetActorLocation(FrontIndicator->GetComponentLocation());
			MulticastRPC_PalletFall(Actor, FrontIndicator->GetComponentLocation());
		}
		else
		{
			// Actor->SetActorLocation(BackIndicator->GetComponentLocation());
			MulticastRPC_PalletFall(Actor, BackIndicator->GetComponentLocation());
		}
		// 킬러가 판자에 맞으면, 기절 상태로 만든다.
		AKiller* Killer = Cast<AKiller>(Actor);
		if (Killer)
		{
			// Stun 은 multicast
			Killer->Stun();
		}
	}
}

void APallet::DebugOwner()
{
	// debug owner
	DrawDebugString(GetWorld(), GetActorLocation(), GetOwner() ? GetOwner()->GetName() : TEXT("None"), nullptr, FColor::Red, 0.0f, true);
}

void APallet::MulticastRPC_PalletFall_Implementation(AActor* Actor, FVector Position)
{
	Actor->SetActorLocation(Position);
	WallComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void APallet::ServerRPC_PalletFall_Implementation()
{
	PalletFall();
}
