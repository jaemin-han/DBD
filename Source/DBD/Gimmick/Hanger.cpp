// Fill out your copyright notice in the Description page of Project Settings.


#include "Hanger.h"

#include "Blueprint/UserWidget.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/HangerUI.h"

// Sets default values
AHanger::AHanger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	HangPosition = CreateDefaultSubobject<UArrowComponent>(TEXT("HangPosition"));
	HangPosition->SetupAttachment(RootComponent);
}

void AHanger::SetHangSurvivor(ADBD_Player* InHangSurvivor)
{
	// InHangSurvivor 가 IsLocallyControlled 이면 UI 를 viewport 에 추가
	if (InHangSurvivor->IsLocallyControlled())
	{
		HangerUI->AddToViewport();
	}
	
	
	HangSurvivor = InHangSurvivor;
}

// Called when the game starts or when spawned
void AHanger::BeginPlay()
{
	Super::BeginPlay();

	// 사용할 UI 인스턴스 생성
	HangerUI = CreateWidget<UHangerUI>(GetWorld(), HangerUIClass);
}

// Called every frame
void AHanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 갈고리에 생존자가 걸렸을 때
	if (!HangSurvivor)
		return;

	float NewSacrifaceTime = HangSurvivor->GetSacrificeTime() - DeltaTime;

	if (NewSacrifaceTime <= 0.f)
	{
		// todo: 생존자 사망 처리
		// delegate 사용할까??
	}

	HangSurvivor->SetSacrificeTime(NewSacrifaceTime);

	// HangerUI 가 유효하고
	// HangerUI 가 Viewport 에 추가되어있을 때
	if (HangerUI && HangerUI->IsInViewport())
	{
		HangerUI->SetSacrifaceGagePercent(NewSacrifaceTime / HangSurvivor->TotalSacrificeTime);
	}
}

void AHanger::Interaction(AActor* Caller)
{
	AKiller* Killer = Cast<AKiller>(Caller);
	if (Killer && Killer->CarriedSurvivor)
	{
		Killer->ServerRPC_HangSurvivorOnHook();
	}

	ADBD_Player* Survivor = Cast<ADBD_Player>(Caller);
	// 상호작용한 액터가 생존자이고, 갈고리에 다른 생존자가 걸려있을 경우
	if (Survivor && HangSurvivor)
	{
		// todo:
		Rescue();
	}
}

void AHanger::FailedInteraction()
{
}

FString AHanger::GetGimmickName()
{
	return Name;
}

FString AHanger::GetInteractKey()
{
	return InteractKey;
}

void AHanger::Rescue()
{
	// 체력 2로 변경
	HangSurvivor->ChangeSurvivorState(ESurvivorState::Hp2);
	// 갈고리에서 해방
	HangSurvivor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	// HangSurvivor의 충돌 판정 복구
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(HangSurvivor->GetRootComponent());
	USkeletalMeshComponent* SkeletalMeshComponent = HangSurvivor->GetMesh();
	if (PrimitiveComponent && SkeletalMeshComponent)
	{
		PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	// HangSurvivor의 CharacterMovement 변경
	HangSurvivor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// HangSurvivor 가 local 인 경우 UI 제거
	if (HangSurvivor->IsLocallyControlled() && HangerUI)
	{
		HangerUI->RemoveFromParent();
	}
	
	HangSurvivor = nullptr;
}
