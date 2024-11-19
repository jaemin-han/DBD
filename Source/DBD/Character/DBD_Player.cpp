// Fill out your copyright notice in the Description page of Project Settings.


#include "DBD_Player.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"

void ADBD_Player::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	// Interation 함수를 0.2초마다 호출
	FTimerHandle interactionTimer;
	GetWorld()->GetTimerManager().SetTimer(interactionTimer, this, &ADBD_Player::Interaction, 0.2f, true);
}

void ADBD_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADBD_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlusHpAction, ETriggerEvent::Started, this, &ADBD_Player::PlusHp);
		EnhancedInputComponent->BindAction(MinusHpAction, ETriggerEvent::Started, this, &ADBD_Player::MinusHp);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ADBD_Player::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ADBD_Player::RunStop);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ADBD_Player::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ADBD_Player::CrouchStop);
	}
}

void ADBD_Player::PlusHp()
{
	UpdateHP(-1);
}

void ADBD_Player::MinusHp()
{
	UpdateHP(1);
}

void ADBD_Player::Interaction()
{
	FVector startPos = GetActorLocation();
	FVector endPos = startPos + GetActorForwardVector() * 100.0f;

	FHitResult hitResult;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, collisionParams);

	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);
}

void ADBD_Player::Run()
{
	// Health가 1이면 리턴
	if (Health == 1) return;

	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

void ADBD_Player::RunStop()
{
	// Health가 1이면 리턴
	if (Health == 1) return;

	IsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = 226.0f;
}

void ADBD_Player::Crouch()
{
	// Health가 3일때만 작동
	if (Health != 3) return;

	IsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = 113.0f;
}

void ADBD_Player::CrouchStop()
{
	// Health가 3일때만 작동
	if (Health != 3) return;

	IsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = 226.0f;
}

void ADBD_Player::UpdateHP(int32 Value)
{
	// Value : Damage or Heal
	Health -= Value;

	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
	else if (Health < 0)
	{
		Health = 0;
	}


	// 3 : WalkSpeed 226, RunSpeed : 400
	// crouch : WalkSpeed 113
	// 2 : WalkSpeed 226, RunSpeed : 400
	// 1 : WalkSpeed 70
	// 피격시 2초 스피드 : 600

	if (Health == 3)
	{
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
	}
	else if (Health == 2)
	{
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
	}
	else if (Health == 1)
	{
		GetCharacterMovement()->MaxWalkSpeed = 70.0f;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health : %d"), Health));
}

void ADBD_Player::UpdateSpeed()
{
	switch (Health)
	{
	case 3:
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
		break;
	case 2:
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
		break;
	case 1:
		GetCharacterMovement()->MaxWalkSpeed = 70.0f;
		break;
	default:
		break;
	}
}
