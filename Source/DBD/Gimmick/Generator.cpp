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
	GaugeUI->UpdateGauge(GetWorld()->DeltaTimeSeconds);
	UE_LOG(LogTemp, Log, TEXT("Random %f"), GetWorld()->DeltaTimeSeconds); 
	// 120프레임 -> 0.008초 -> random
	// 60 프레임 -> 0.016초 -> random
	// 30 프레임 -> 0.033초 -> random
	if (not IsRoundGauge)
	{
		float random = FMath::RandRange(0.0f, 1.0f); // 1%
		UE_LOG(LogTemp, Log, TEXT("Random %f"), random);
		//
		if (random < GetWorld()->DeltaTimeSeconds * 0.1f)
		{
			IsRoundGauge = true;
		}
	}

	UpdateRoundGauge();
}

void AGenerator::FailedInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("Generator FailedInteraction"));
	GaugeUI->SetVisibility(ESlateVisibility::Hidden);
	IsRoundGauge = false;
}

void AGenerator::UpdateRoundGauge()
{
	if (IsRoundGauge)
	{
		GaugeUI->VisibleRondGauge(true);
		if (GaugeUI->UpdateRoundPercent(GetWorld()->DeltaTimeSeconds))
		{
			IsRoundGauge = false;
		}
	}
	else
	{
		GaugeUI->VisibleRondGauge(false);
	}
}

