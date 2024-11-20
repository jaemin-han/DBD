// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Windows.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AWindows::AWindows()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WindowFrameMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindowFrameMeshComp"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> windowFrameMeshCompAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (windowFrameMeshCompAsset.Succeeded())
	{
		WindowFrameMeshComp->SetStaticMesh(windowFrameMeshCompAsset.Object);
	}

	WindowFrameMeshComp->SetRelativeScale3D(FVector(0.3f, 1.0f, 1.0f));
}

// Called when the game starts or when spawned
void AWindows::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWindows::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWindows::Interaction(bool IsExec)
{
	UE_LOG(LogTemp, Warning, TEXT("Windows Interaction"));
}

