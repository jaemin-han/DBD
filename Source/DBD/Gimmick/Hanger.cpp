// Fill out your copyright notice in the Description page of Project Settings.


#include "Hanger.h"


// Sets default values
AHanger::AHanger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHanger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHanger::Interaction(AActor* Caller)
{
}

