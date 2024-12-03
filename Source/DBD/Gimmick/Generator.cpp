// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Generator.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
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

void AGenerator::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGenerator, IsFullGauge);
}

// Called every frame
void AGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// drawdebugstring: Owner
	FString OwnerName = GetOwner() ? GetOwner()->GetName() : TEXT("None");
	DrawDebugString(GetWorld(), GetActorLocation(), OwnerName, nullptr, FColor::Green, DeltaTime);

	// 서버에서만 실행
	if (!HasAuthority()) return;

	if (GetIsFullGauge())
	{
		IsActivated = true;
	}
}

void AGenerator::Interaction(APawn* Caller)
{
	SetOwner(Caller);
	
	if (Caller)
	{
		// Caller client RPC
		ClientRPC_SetGaugeUIVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Generator Interaction Caller is nullptr"));
	}

	UpdateGauge(GetWorld()->GetDeltaSeconds());

	
	//UE_LOG(LogTemp, Log, TEXT("Random %f"), GetWorld()->DeltaTimeSeconds); 
	// 120프레임 -> 0.008초 -> random
	// 60 프레임 -> 0.016초 -> random
	// 30 프레임 -> 0.033초 -> random
}

void AGenerator::FailedInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("Generator FailedInteraction"));
	ClientRPC_SetGaugeUIVisibility(ESlateVisibility::Hidden);
	IsRoundGauge = false;
	SetOwner(nullptr);
}

FString AGenerator::GetGimmickName()
{
	return Name;
}

FString AGenerator::GetInteractKey()
{
	return InteractKey;
}

// 서버에서만 실행됨
void AGenerator::UpdateGauge(float time)
{
	if (Percent >= 1.0f)
	{
		IsFullGauge = true;
		return;
	}
	
	// RoundGauge가 활성화 되어있다면 리턴 시켜주기
	//if (ActivatedRoundGauge) return;


	// time = 0.2로 들어오는데 이거를 100퍼로 나눴을때
	// 초당 0.01씩 증가하게 만들기 위해서
	// Percent += time * 0.011f;		// 실제 적용
	Percent += time * 0.5f;			// 테스트용

	MultiRPC_SetGaugeUIPercent(Percent);
}

void AGenerator::ClientRPC_SetGaugeUIVisibility_Implementation(ESlateVisibility visibility)
{
	if (GaugeUI)
	{
		GaugeUI->SetVisibility(visibility);
	}
}

void AGenerator::MultiRPC_SetGaugeUIPercent_Implementation(float value)
{
	GaugeUI->SetPercent(value);
}