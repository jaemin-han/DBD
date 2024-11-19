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
	ConstructorHelpers::FObjectFinder<UStaticMesh> boardMeshCompAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (boardMeshCompAsset.Succeeded())
	{
		BoardMeshComp->SetStaticMesh(boardMeshCompAsset.Object);
	}

	BoardMeshComp->SetRelativeScale3D(FVector(1.0f, 0.3f, 1.0f));
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

