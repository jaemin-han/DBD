// Fill out your copyright notice in the Description page of Project Settings.


#include "Hanger.h"

#include "Blueprint/UserWidget.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/DBDGameInstance.h"
#include "GameMode/DBDGameState.h"
#include "GameMode/DBDPlayerController.h"
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

	SetReplicates(true);
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

	if (HangerUI && HangerUI->IsInViewport() && HangSurvivor->IsLocallyControlled())
	{
		HangerUI->SetSacrifaceGagePercent(HangSurvivor->GetSacrificeTime() / HangSurvivor->TotalSacrificeTime);
	}

	// 서버에서만 실행
	if (!HasAuthority())
		return;


	float NewSacrifaceTime = HangSurvivor->GetSacrificeTime() - DeltaTime;

	if (NewSacrifaceTime <= 0.f)
	{
		ServerRPC_DestroyHangSurvivor();
		return;
	}

	HangSurvivor->SetSacrificeTime(NewSacrifaceTime);
}

void AHanger::Interaction(APawn* Caller)
{
	AKiller* Killer = Cast<AKiller>(Caller);
	if (Killer && Killer->CarriedSurvivor)
	{
		Killer->ServerRPC_HangSurvivorOnHook();
		return;
	}

	ADBD_Player* Survivor = Cast<ADBD_Player>(Caller);
	// 상호작용한 액터가 생존자이고, 갈고리에 다른 생존자가 걸려있을 경우
	if (Survivor && HangSurvivor)
	{
		// todo:
		Rescue();
	}
	else
	{
		// server 인지 client 인지 log
		FString NetRole = HasAuthority() ? TEXT("Server") : TEXT("Client");
		UE_LOG(LogTemp, Error, TEXT("Rescue Interaction Failed at %s"), *NetRole);
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

void AHanger::MultiRPC_DestroyHangSurvivor_Implementation()
{
	if (HangSurvivor->IsLocallyControlled() && HangerUI)
	{
		HangerUI->RemoveFromParent();
	}
	HangSurvivor = nullptr;
}

void AHanger::ServerRPC_DestroyHangSurvivor_Implementation()
{
	if (HangSurvivor)
	{
		// game instance setting - for game over UI
		auto* GameInstance = Cast<UDBDGameInstance>(GetWorld()->GetGameInstance());
		GameInstance->SetKillerKillCount(GameInstance->GetKillerKillCount() + 1);
		
		auto* GameState = Cast<ADBDGameState>(GetWorld()->GetGameState());
		GameState->SetSurvivorCount(GameState->GetSurvivorCount() - 1);
		if (GameState->GetSurvivorCount() == 0)
		{
			HangSurvivor->Destroy();
			MultiRPC_DestroyHangSurvivor();
			GameState->MultiRPC_GameOver();
		}
		else
		{
			auto* PlayerController = Cast<ADBDPlayerController>(HangSurvivor->GetController());
			MultiRPC_DestroyHangSurvivor();
			PlayerController->ServerRPC_ChangeToSpector();
		}
	}
}
