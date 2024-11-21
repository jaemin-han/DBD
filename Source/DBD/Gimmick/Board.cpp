// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Board.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABoard::ABoard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoardMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMeshComp"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> boardMeshCompAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/Source/SM_Board.SM_Board'"));
	if (boardMeshCompAsset.Succeeded())
	{
		BoardMeshComp->SetStaticMesh(boardMeshCompAsset.Object);
	}
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

	CurrentRoll = GetActorRotation().Roll;
	TargetRoll = CurrentRoll - 80.0f;
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFallen)
		return;

	if (!bIsInteracted)
		return;

	CurrentRoll -= 180.0f * DeltaTime;
	BoardMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, CurrentRoll));

	if (CurrentRoll - TargetRoll < KINDA_SMALL_NUMBER)
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
}

void ABoard::FailedInteraction()
{
}
