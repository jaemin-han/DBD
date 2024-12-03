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
	if (GetOwner())
	{
		DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("GetOwner : %s"), *GetOwner()->GetName()), nullptr, FColor::Red, DeltaTime);
	}
	else
	{
		DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("No")), nullptr, FColor::Red, DeltaTime);

	}

	if (not HasAuthority()) return;


	OpenExitDoor();



	for (AGenerator* generator : Generators)
	{
		if (not generator->IsActivated) return;
	}

	IsDoorActivated = true;
}

void ADoor::Interaction(APawn* Caller)
{
	if (not IsDoorActivated) return;
	if (not ExitGaugeUI) return;
	UE_LOG(LogTemp, Log, TEXT("[local] Door Interaction"));
	//Client_InteractDoor();
	
	ExitGaugeUI->SetVisibility(ESlateVisibility::Visible);
	ExitGaugeUI->Server_UpdateExitGauge();
}

void ADoor::FailedInteraction()
{
	if (not IsDoorActivated) return;
	if (not ExitGaugeUI) return;
	
	UE_LOG(LogTemp, Log, TEXT("Door Failed Interaction"));
	ExitGaugeUI->SetVisibility(ESlateVisibility::Hidden);
}
void ADoor::Server_InteractDoor_Implementation(APawn* Caller)
{
	
	//Multi_SetOwner(Caller);

	Interaction(Caller);
}

void ADoor::Client_InteractDoor_Implementation(APawn* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("[Client] Door Interaction"));
	//ExitGaugeUI->SetVisibility(ESlateVisibility::Visible);
	//ExitGaugeUI->UpdateExitGauge(GetWorld()->DeltaTimeSeconds);
}


FString ADoor::GetGimmickName()
{
	return Name;
}

FString ADoor::GetInteractKey()
{
	return InteractKey;
}

void ADoor::Multi_SetOwner_Implementation(AActor* Caller)
{
	if (Caller) SetOwner(Caller);
}

void ADoor::OpenExitDoor()
{
	if (not IsDoorActivated) return;

	if (DoorMesh->GetRelativeLocation().Z >= 400.0f) return;
	//if (GetOwner())
	//{
	//	if (HasAuthority())
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("[%s] MultiServer OpenExitDoor"), *GetOwner()->GetName());
	//
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Log, TEXT("[%s] MultiClient OpenExitDoor"), *GetOwner()->GetName());
	//	}
	//}
	
	if (ExitGaugeUI and ExitGaugeUI->GetIsFullGauge())
	{
		Multi_OpenExitDoor();
	}
}

void ADoor::Server_OpenExitDoor_Implementation()
{
	OpenExitDoor();
}

void ADoor::Multi_OpenExitDoor_Implementation()
{
	DoorMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ExitCol->SetCollisionProfileName(TEXT("NoCollision"));
	

	FVector Movloc = DoorMesh->GetRelativeLocation() + GetActorUpVector();
	UE_LOG(LogTemp, Log, TEXT("[%s] Local Movloc X : %.2f, Y : %.2f, Z : %.2f"),
		*GetOwner()->GetName(), Movloc.X, Movloc.Y, Movloc.Z);


	DoorMesh->SetRelativeLocation(Movloc);
}