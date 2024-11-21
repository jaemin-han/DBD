// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Generator.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "UI/GaugeUI.h"


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
	
	if (GaugeUIWidgetClass)
	{
		GaugeUI = Cast<UGaugeUI>(CreateWidget(GetWorld(), GaugeUIWidgetClass));
		if (GaugeUI)
		{
			GaugeUI->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("GaugeUI Create Success"));

			GaugeUI->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGenerator::Interaction()
{
	UE_LOG(LogTemp, Warning, TEXT("Generator Interaction"));
	GaugeUI->SetVisibility(ESlateVisibility::Visible);
	GaugeUI->UpdateGauge(0.2f);
}

void AGenerator::FailedInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("Generator FailedInteraction"));
	GaugeUI->SetVisibility(ESlateVisibility::Hidden);
}

