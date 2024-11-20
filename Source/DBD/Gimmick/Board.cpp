// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Board.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABoard::ABoard()
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

	BoardMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMeshComp"));
	BoardMeshComp->SetupAttachment(RootComponent);

	// Front, Back Indicator
	FrontIndicator = CreateDefaultSubobject<USphereComponent>(TEXT("FrontIndicator"));
	FrontIndicator->SetupAttachment(RootComponent);
	BackIndicator = CreateDefaultSubobject<USphereComponent>(TEXT("BackIndicator"));
	BackIndicator->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

	CurrentRoll = GetActorRotation().Roll;
	TargetRoll = CurrentRoll + 60.0f;
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFallen)
		return;

	if (!bIsInteracted)
		return;

	CurrentRoll += 180.0f * DeltaTime;
	BoardMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, CurrentRoll));

	if (TargetRoll - CurrentRoll < KINDA_SMALL_NUMBER)
	{
		bIsFallen = true;
		bIsInteracted = false;
		BoardMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, TargetRoll));
	}
}

void ABoard::Interaction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Board Interaction"));
	bIsInteracted = true;

	TArray<AActor*> OverlappingActors;
	WallComp->GetOverlappingActors(OverlappingActors);

	// OverlappingActors 이 비었는지 확인
	if (OverlappingActors.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappingActors is Empty"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappingActors is not Empty"));
	}
	
	for (AActor* Actor : OverlappingActors)
	{
		float FrontDistance = FVector::Dist(FrontIndicator->GetComponentLocation(), Actor->GetActorLocation());
		float BackDistance = FVector::Dist(BackIndicator->GetComponentLocation(), Actor->GetActorLocation());
		if (FrontDistance < BackDistance)
		{
			Actor->SetActorLocation(FrontIndicator->GetComponentLocation());
			// debug ue_log
			UE_LOG(LogTemp, Warning, TEXT("FrontDistance: %f"), FrontDistance);
		}
		else
		{
			Actor->SetActorLocation(BackIndicator->GetComponentLocation());
			// debug ue_log
			UE_LOG(LogTemp, Warning, TEXT("BackDistance: %f"), BackDistance);
		}
	}

	WallComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

}
