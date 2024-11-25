// Fill out your copyright notice in the Description page of Project Settings.


#include "Killer.h"
#include "Killer.h"

#include "DBD_Player.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gimmick/DBD_Interface_Gimmick.h"
#include "Gimmick/Hanger.h"


// Sets default values
AKiller::AKiller()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// mesh
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// detach camera from spring arm
	FollowCamera->SetupAttachment(GetMesh(), FName("head"));
	FollowCamera->SetRelativeLocation(FVector(1.736f, 9.84f, 0.0f));
	FollowCamera->SetRelativeRotation(FRotator(0.0, 80.0, -90.0));
	FollowCamera->bUsePawnControlRotation = true;

	// weapon static mesh component
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("WeaponSocket_l"));

	// weapon no collision preset
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// sphere collision component for search gimmick
	SearchGimmickSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SearchGimmickSphere"));
	SearchGimmickSphere->SetupAttachment(RootComponent);
	SearchGimmickSphere->SetSphereRadius(100.0f);

	SetActorScale3D(FVector(1.2f));
}

// Called when the game starts or when spawned
void AKiller::BeginPlay()
{
	Super::BeginPlay();
}

void AKiller::Attack()
{
	if (bStunned)
		return;

	UE_LOG(LogTemp, Display, TEXT("Attack"));
	// play montage
	PlayAnimMontage(KillerMontage, 1.0f, FName("Attack"));
}

// Called every frame
void AKiller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetNearGimmick();
	Debug();
}

// Called to bind functionality to input
void AKiller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// bind attack action
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AKiller::Attack);

	// InteractionAction 에 대한 바인딩 추가
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &AKiller::Interact);
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &AKiller::CarrySurvivor);
	EnhancedInputComponent->BindAction(DropDownSurvivorAction, ETriggerEvent::Started, this,
	                                   &AKiller::DropDownSurvivor);
}

void AKiller::GetNearGimmick()
{
	// SearchGimmickSphere 와 겹치는 엑터 중, IDBD_Interface_Gimmick 인터페이스를 구현한 엑터를 찾아 NearGimmick 에 할당
	TArray<AActor*> OverlappingActors;
	SearchGimmickSphere->GetOverlappingActors(OverlappingActors);
	AActor* NewNearGimmick = nullptr;
	ADBD_Player* NewNearSurvivor = nullptr;
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->GetClass()->ImplementsInterface(UDBD_Interface_Gimmick::StaticClass()))
		{
			NewNearGimmick = OverlappingActor;
		}

		NewNearSurvivor = Cast<ADBD_Player>(OverlappingActor);
	}
	NearGimmick = NewNearGimmick;
	NearSurvivor = NewNearSurvivor;
}

void AKiller::Debug()
{
	// NearGimmick 에 무엇이 할당되어 있는지 확인 후 스크린에 출력
	FString DebugString = NearGimmick.GetObject() ? NearGimmick.GetObject()->GetName() : TEXT("None");
	GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Red, DebugString);
	// NearSurvivor 에 무엇이 할당되어 있는지 확인 후 스크린에 출력
	DebugString = NearSurvivor ? NearSurvivor->GetName() : TEXT("None");
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, DebugString);
}

void AKiller::Stun()
{
	// Stun 몽타주 실행
	bStunned = true;
	PlayAnimMontage(KillerMontage, 2.0f, FName("Stun"));
}

void AKiller::DestroyPallet()
{
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("DestroyPallet"));
	PlayAnimMontage(KillerMontage, 0.5f, FName("DestroyPallet"));
}

void AKiller::Interact()
{
	// NearGimmick 이 유효한 경우 Interaction 함수 호출
	if (NearGimmick.GetObject() && !bStunned)
	{
		NearGimmick->Interaction(this);
	}
}

void AKiller::CarrySurvivor()
{
	// 근처에 생존자가 있고, 해당 생존자의 체력이 1인 경우 생존자를 옮길 수 있음
	if (NearSurvivor && NearSurvivor->GetHealth() == 1 && CarriedSurvivor == nullptr)
	{
		/*
		*	TODO: 생존자의 상태를 바꿔서, 생존자에서 애니메이션 관리를 자동으로 하게 하는게 나을듯?
		*	예를 들어 생존자의 상태를 enum으로 관리하고, 해당 상태에 따라 애니메이션을 변경하도록 하는 방식
		*	이 함수를 진행하면 생존자의 상태를 업힘? 으로 변경하고 
		*/

		// todo: 생존자 상태 변경
		NearSurvivor->ChangeSurvivorState(ESurvivorState::Piggyback);

		CarriedSurvivor = NearSurvivor;
		CarriedSurvivor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		                                   CarrySocketName);

		// 충돌 판정을 담당하는 capsule component 와 skeletal mesh component 의 충돌을 끔
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(CarriedSurvivor->GetRootComponent());
		USkeletalMeshComponent* SkeletalMeshComponent = CarriedSurvivor->GetMesh();
		if (PrimitiveComponent && SkeletalMeshComponent)
		{
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		// 생존자의 CharacterMovementComponent 의 MovementMode 를 None 으로 변경
		CarriedSurvivor->GetCharacterMovement()->DisableMovement();
	}
}

void AKiller::DropDownSurvivor()
{
	UE_LOG(LogTemp, Display, TEXT("DropDownSurvivor"));
	// 옮기고 있는 생존자가 있을 경우 해당 생존자를 놓음
	if (CarriedSurvivor)
	{
		CarriedSurvivor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		// 충돌 판정을 담당하는 capsule component 와 skeletal mesh component 의 충돌을 켬
		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(CarriedSurvivor->GetRootComponent());
		USkeletalMeshComponent* SkeletalMeshComponent = CarriedSurvivor->GetMesh();
		if (PrimitiveComponent && SkeletalMeshComponent)
		{
			PrimitiveComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		// 생존자의 CharacterMovementComponent 의 MovementMode 를 Walking 으로 변경
		CarriedSurvivor->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		CarriedSurvivor->ChangeSurvivorState(ESurvivorState::Hp1);
		CarriedSurvivor = nullptr;
	}
}

void AKiller::HangSurvivorOnHook()
{
	// Todo: 에니메이션이 추가된다면, 해당 에니메이션을 실행하고, 에니메이션이 끝나면 아래 코드를 실행하도록 수정
	AHanger* Hanger = Cast<AHanger>(NearGimmick.GetObject());
	if (!Hanger)
		return;
	
	if (CarriedSurvivor && Hanger)
	{
		PlayAnimMontage(KillerMontage, 1.0f, FName("HangSurvivorOnHook"));

		// AN_HangOnHook.cpp 에서 처리하도록 수정
		// CarriedSurvivor->ChangeSurvivorState(ESurvivorState::Hang);
		// CarriedSurvivor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		// CarriedSurvivor->AttachToComponent(Hanger->HangPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
		// CarriedSurvivor = nullptr;
	}
}
