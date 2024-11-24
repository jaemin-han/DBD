// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Door.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UI/ExitGaugeUI.h"
#include "Gimmick/Generator.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExitMesh"));
	SetRootComponent(ExitMesh);

	GaugeCol = CreateDefaultSubobject<UBoxComponent>(TEXT("GaugeCol"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	ExitCol = CreateDefaultSubobject<UBoxComponent>(TEXT("ExitCol"));

	GaugeCol->SetupAttachment(ExitMesh);
	DoorMesh->SetupAttachment(ExitMesh);
	ExitCol->SetupAttachment(DoorMesh);
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (ExitGaugeUIClass)
	{
		ExitGaugeUI = Cast<UExitGaugeUI>(CreateWidget(GetWorld(), ExitGaugeUIClass));
		if (ExitGaugeUI)
		{
			ExitGaugeUI->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("ExitGaugeUI Create Success"));

			ExitGaugeUI->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// 월드의 발전기 찾아두기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGenerator::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		Generators.Add(Cast<AGenerator>(Actor));
	}
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ExitGaugeUI and ExitGaugeUI->GetIsFullGauge())
	{
		OpenExitDoor();
	}

	for (AGenerator* generator : Generators)
	{
		if (not generator->IsActivated) return;
	}

	IsDoorActivated = true;
}

void ADoor::Interaction(AActor* Caller)
{
	UE_LOG(LogTemp, Log, TEXT(" Door Interaction"));
	if (not IsDoorActivated) return;
	if (not ExitGaugeUI) return;

	ExitGaugeUI->SetVisibility(ESlateVisibility::Visible);
	ExitGaugeUI->UpdateExitGauge(GetWorld()->DeltaTimeSeconds);
}

void ADoor::FailedInteraction()
{
	if (not IsDoorActivated) return;
	if (not ExitGaugeUI) return;
	
	UE_LOG(LogTemp, Log, TEXT("Door Failed Interaction"));
	ExitGaugeUI->SetVisibility(ESlateVisibility::Hidden);
}

void ADoor::OpenExitDoor()
{
	if (not IsDoorActivated) return;

	if (DoorMesh->GetRelativeLocation().Z >= 400.0f) return;

	//DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// DorrMesh의 콜리전을 NonCollision으로 변경
	//DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ExitCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DoorMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ExitCol->SetCollisionProfileName(TEXT("NoCollision"));

	UE_LOG(LogTemp, Log, TEXT("OpenExitDoor"));
	FVector Movloc = DoorMesh->GetRelativeLocation() + GetActorUpVector();
	DoorMesh->SetRelativeLocation(Movloc);
}

