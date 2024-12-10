// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Generator.h"

#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/DBD_Player.h"


#include "UI/GaugeUI.h"
#include "UI/RoundGaugeUI.h"
#include "UI/SkillCheckZoneUI.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"


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

	GeneratorSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("GeneratorSoundComp"));
	GeneratorSoundComp->SetVolumeMultiplier(0.0f);
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
// 서버에서만 실행됨
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
	//IsCheckRoundGauge = false;
	//RoundPercent = 0.0f;
	//GeneratorSoundComp->SetVolumeMultiplier(0.0f);
	//GeneratorSoundComp->Stop();
	//GeneratorSoundTimer = 0.0f;
	//SetOwner(nullptr);

	Multi_FailedInteract();
}

FString AGenerator::GetGimmickName()
{
	return Name;
}

FString AGenerator::GetInteractKey()
{
	return InteractKey;
}	

void AGenerator::Multi_FailedInteract_Implementation()
{
	IsCheckRoundGauge = false;
	RoundPercent = 0.0f;
	GeneratorSoundComp->SetVolumeMultiplier(0.0f);
	GeneratorSoundComp->Stop();
	GeneratorSoundTimer = 0.0f;
	SetOwner(nullptr);
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
	 Percent += time * 0.011f * RepairMultiplier;		// 실제 적용
	//Percent += time * 0.5f;			// 테스트용
	 
	// RoundGaugeUI 계속 업데이트
	if (not IsCheckRoundGauge)
	{
		CheckRoundGauge(GetWorld()->GetDeltaSeconds());
	}
	else
	{
		if (IsSuccessedSkillCheck())
		{
			IsCheckRoundGauge = false;
			RoundPercent = 0.0f;
		}
		
		RoundPercent += GetWorld()->GetDeltaSeconds() * 0.5f;
	}

	Client_VisibleRoundGauge(time, IsCheckRoundGauge);
	Multi_SetRoundGaugePercent(IsCheckRoundGauge, RoundPercent);

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


// 서버에서만 실행됨
void AGenerator::CheckRoundGauge(float frame)
{	
	float random = FMath::RandRange(0.0f,1.0f);
	UE_LOG(LogTemp, Warning, TEXT("Random %f"), random);

	if (random < frame * 0.2f)
	{
		// RoundGauge 활성화
		IsCheckRoundGauge = true;

		float skillCheckRandom = FMath::RandRange(0.4f, 0.7f);
		SetSkillCheckZone(skillCheckRandom);
	}
	else
	{
		IsCheckRoundGauge = false;
	}
}

void AGenerator::SetSkillCheckZone(float value)
{
	SkillCheckZoneStart = value;
	
	//UE_LOG(LogTemp, Log, TEXT("value %f"), value);
	
	float Random = FMath::RandRange(0.1f, 0.2f);
	//UE_LOG(LogTemp, Log, TEXT("Random %f"), Random);
	GaugeUI->SkillCheckZone->SetPercent(Random);
	SkillCheckZoneEnd = SkillCheckZoneStart + Random;

	// 0 ~ 1의 범위를 -180 ~ 180의 범위로 변환
	//SkillCheckZoneStart = ((value * 2) - 1) * 180.0f;
	float angle = ((value * 2) - 1) * 180.0f;
	// SKillCheckZone의 RenderTransform의 Angle을 변경
	//SkillCheckZone->RenderTransform.Angle = start;
	
	
	FWidgetTransform newTransform = GaugeUI->SkillCheckZone->RenderTransform;
	newTransform.Angle = angle;
	GaugeUI->SkillCheckZone->SetRenderTransform(newTransform);
	
	GaugeUI->SkillCheckZone->InvalidateLayoutAndVolatility();

	Multi_SetSkillCheckZone(Random, newTransform);
}

void AGenerator::Multi_SetSkillCheckZone_Implementation(float ran, FWidgetTransform newTrans)
{
	GaugeUI->SkillCheckZone->SetPercent(ran);
	GaugeUI->SkillCheckZone->SetRenderTransform(newTrans);
}

void AGenerator::Client_VisibleRoundGauge_Implementation(float time, bool IsVisible)
{
	GeneratorSoundTimer -= time;
	if (GeneratorSoundTimer <= 0.0f)
	{
		GeneratorSoundTimer = 0.5f;
		GeneratorSoundComp->SetVolumeMultiplier(1.0f);
		//GeneratorSoundComp->Stop();
		GeneratorSoundComp->Play();
		GeneratorSoundTimer = 7.0f;
	}

	if (IsVisible)
	{
		GaugeUI->RoundGauge->SetVisibility(ESlateVisibility::Visible);
		GaugeUI->SkillCheckZone->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GaugeUI->RoundGauge->SetVisibility(ESlateVisibility::Hidden);
		GaugeUI->SkillCheckZone->SetVisibility(ESlateVisibility::Hidden);
	}
}


void AGenerator::Multi_SetRoundGaugePercent_Implementation(bool IsVisible, float per)
{
	if (IsVisible)
	{
		GaugeUI->RoundGauge->UpdatePercent(per);
	}
}



bool AGenerator::IsSuccessedSkillCheck()
{
	// 게이지가 스킬체크 구역에 들어왔을때
	if (RoundPercent >= SkillCheckZoneStart and RoundPercent <= SkillCheckZoneEnd)
	{
		
		if (Cast<ADBD_Player>(GetOwner())->GetIsSpaceBar())
		{
			IsSuccessSkillCheck = true;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("SkillCheckZoneStart : %.2f"), SkillCheckZoneStart);
	UE_LOG(LogTemp, Log, TEXT("RoundPercent : %.2f"), RoundPercent);
	UE_LOG(LogTemp, Log, TEXT("SkillCheckZoneEnd : %.2f"), SkillCheckZoneEnd);

 // 플레이어가 스페이스바를 눌렀는데 스킬체크에 성공했을때
	if (Cast<ADBD_Player>(GetOwner())->GetIsSpaceBar() and IsSuccessSkillCheck)
	{
		UE_LOG(LogTemp, Log, TEXT("Success SkillCheck"));
		RoundPercent = 0.0f;
		IsSuccessSkillCheck = false;
		return true;
	}
	// 게이지가 다 찼거나, 플레이거 스페이스바를 눌렀는데 스킬체크에 실패했을때
	else if (RoundPercent >= 1.0f or (Cast<ADBD_Player>(GetOwner())->GetIsSpaceBar() and IsSuccessSkillCheck == false))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed SkillCheck"));
		IsSuccessSkillCheck = false;		// 스킬체크성공여부는 다시 false로
		RoundPercent = 0.0f;			// 게이지 초기화
		if (Percent <= 0.02f)				// 게이지가 0.02보다 작다면 그냥 0으로 초기화
		{
			Percent = 0;
		}
		else Percent -= 0.02f;

		// 델리게이트 호출
		if (OnGenerateFail.IsBound())
		{
			OnGenerateFail.Execute();
		}

		return true;
	}
	else return false;
}