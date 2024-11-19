// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Generator.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AGenerator::AGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GeneratorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GeneratorMeshComp"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> generatorMeshCompAsset(TEXT("/Script/Engine.StaticMesh'/Game/DBD/Object/Generator/Generator.Generator'"));
	if (generatorMeshCompAsset.Succeeded())
	{
		GeneratorMeshComp->SetStaticMesh(generatorMeshCompAsset.Object);
	}


}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

