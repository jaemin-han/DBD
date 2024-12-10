// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Door.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ProgressBar.h"
#include "UI/ExitGaugeUI.h"
#include "Gimmick/Generator.h"
#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h"
#include "Character/DBD_Player.h"



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
		if (generator->IsActivated)
		{
			GeneratorClearCount++;
		}
	}

	if (GeneratorClearCount < Generators.Num() - 2) return;

	IsDoorActivated = true;
}

void ADoor::Interaction(APawn* Caller)
{
	if (not IsDoorActivated) return;
	if (not ExitGaugeUI) return;
	UE_LOG(LogTemp, Log, TEXT("[local] Door Interaction"));
	//Client_InteractDoor();
	
	//ExitGaugeUI->SetVisibility(ESlateVisibility::Visible);
	//ExitGaugeUI->UpdateExitGauge(GetWorld()->GetDeltaSeconds());
	if (ExitGaugeUI->GetIsFullGauge())
	{
		ExitGaugeUI->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	Client_InteractDoor(Caller);

	//Cast<APlayerController>(Caller->GetController());

	//ExitGaugeUI->SetOwningPlayer(Cast<APlayerController>(Caller->GetOwner()));

	//ExitGaugeUI->SetOwningPlayer(pc);


	UpdateExitGauge();

	//ExitGaugeUI->UpdateExitGauge(GetWorld()->GetDeltaSeconds());
}

void ADoor::FailedInteraction()
{
	if (not IsDoorActivated) return;
	if (not ExitGaugeUI) return;
	
	Client_FailedInterat();
}
void ADoor::Server_InteractDoor_Implementation(APawn* Caller)
{
	
	//Multi_SetOwner(Caller);

	Interaction(Caller);
}

void ADoor::Client_InteractDoor_Implementation(APawn* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("[Client] Door Interaction"));
	ExitGaugeUI->SetVisibility(ESlateVisibility::Visible);
	//ExitGaugeUI->UpdateExitGauge(GetWorld()->DeltaTimeSeconds);
}
void ADoor::Multi_InteractDoor_Implementation(AActor* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("[Multi] Door Interaction"));
	ExitGaugeUI->SetVisibility(ESlateVisibility::Visible);
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



// Gauge UI 업데이트 함수들
void ADoor::UpdateExitGauge()
{
	if (ExitGaugeUI->GetGaugePercent() > 1.0f)
	{
		ExitGaugeUI->SetIsFullGauge(true);
		ExitGaugeUI->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("Server_ IsFullGauge : %d"), ExitGaugeUI->GetIsFullGauge());
		return;
	}
	ExitGaugeUI->SetGaugePerent(GetWorld()->GetDeltaSeconds() * 0.625);
	UE_LOG(LogTemp, Warning, TEXT("Server_ Percent : %.2f"), ExitGaugeUI->GetGaugePercent());
	//ExitGauge->SetPercent(Percent);

	Client_UpdateExitGauge(ExitGaugeUI->GetGaugePercent());
}

void ADoor::Client_UpdateExitGauge_Implementation(float persent)
{
	ExitGaugeUI->GetExitGauge()->SetPercent(persent);
}




void ADoor::Client_FailedInterat_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Door Failed Interaction"));
	ExitGaugeUI->SetVisibility(ESlateVisibility::Hidden);
}

// 문 오픈 함수들
void ADoor::OpenExitDoor()
{
	if (not IsDoorActivated) return;

	if (DoorMesh->GetRelativeLocation().Z >= 400.0f) return;
	
	if (ExitGaugeUI and ExitGaugeUI->GetIsFullGauge())
	{
		UE_LOG(LogTemp, Log, TEXT("OpenExitDoor"));
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
	// UE_LOG(LogTemp, Log, TEXT("[%s] Local Movloc X : %.2f, Y : %.2f, Z : %.2f"),
	// 	*GetOwner()->GetName(), Movloc.X, Movloc.Y, Movloc.Z);


	DoorMesh->SetRelativeLocation(Movloc);
}