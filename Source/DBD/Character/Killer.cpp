// Fill out your copyright notice in the Description page of Project Settings.


#include "Killer.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gimmick/DBD_Interface_Gimmick.h"


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
	
}

void AKiller::GetNearGimmick()
{
	// SearchGimmickSphere 와 겹치는 엑터 중, IDBD_Interface_Gimmick 인터페이스를 구현한 엑터를 찾아 NearGimmick 에 할당
	TArray<AActor*> OverlappingActors;
	SearchGimmickSphere->GetOverlappingActors(OverlappingActors);
	AActor* NewNearGimmick = nullptr;
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->GetClass()->ImplementsInterface(UDBD_Interface_Gimmick::StaticClass()))
		{
			NewNearGimmick = OverlappingActor;
			break;
		}
	}
	NearGimmick = NewNearGimmick;
}

void AKiller::Debug()
{
	// NearGimmick 에 무엇이 할당되어 있는지 확인 후 스크린에 출력
	FString DebugString = NearGimmick.GetObject() ? NearGimmick.GetObject()->GetName() : TEXT("None");
	GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Red, DebugString);
	
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

