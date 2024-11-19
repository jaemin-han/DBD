// Fill out your copyright notice in the Description page of Project Settings.


#include "Killer.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


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
	
	SetActorScale3D(FVector(1.5f));
}

// Called when the game starts or when spawned
void AKiller::BeginPlay()
{
	Super::BeginPlay();
}

void AKiller::Attack()
{
	UE_LOG(LogTemp, Display, TEXT("Attack"));
	// play montage
	PlayAnimMontage(AttackMontage);
	
}

// Called every frame
void AKiller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AKiller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// bind attack action
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AKiller::Attack);

	
}
