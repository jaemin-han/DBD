// Fill out your copyright notice in the Description page of Project Settings.


#include "DBD_Player.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"

#include "Gimmick/DBD_Interface_Gimmick.h"

void ADBD_Player::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	// Interation 함수를 0.2초마다 호출
	//FTimerHandle interactionTimer;
	//GetWorld()->GetTimerManager().SetTimer(interactionTimer, this, &ADBD_Player::Interaction, 0.2f, true);
}

void ADBD_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Interaction();
}

void ADBD_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlusHpAction, ETriggerEvent::Started, this, &ADBD_Player::PlusHp);
		EnhancedInputComponent->BindAction(MinusHpAction, ETriggerEvent::Started, this, &ADBD_Player::MinusHp);

		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::Parkour);

		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ADBD_Player::PushInteractGenerator);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Completed, this, &ADBD_Player::NonPushInteractGenerator);


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
	if (bHit)
	{
		if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(hitResult.GetActor()))
		{	
			// HitActor가 Generator라면
			if (hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator"))
			{
				//UE_LOG(LogTemp, Warning, TEXT(" Generator"));
				IsReachGenerator = true;

				if (IsInteractGenerator)
				{
					gimmick->Interaction(); // 게이지 UI 생성 함수
				}
				else
				{
					gimmick->FailedInteraction(); // 게이지 UI 제거 함수
				}
			}
			// HitActor가 Windows라면
			else if (hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Windows"))
			{
				UE_LOG(LogTemp, Warning, TEXT("Windows"));
				IsReachWindows =  true;
			}
			// HitActor가 Board라면
			// HitActor가 출구라면
		}
	}
	else
	{
		IsReachGenerator = false;
		IsReachWindows = false;
		IsInteractGenerator = false;
	}

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

void ADBD_Player::PushInteractGenerator()
{
	if (not IsReachGenerator) return;
	
	IsInteractGenerator = true;
}

void ADBD_Player::NonPushInteractGenerator()
{
	//if (not IsReachGenerator) return;
	//UE_LOG(LogTemp, Warning, TEXT("NonPushInteractGenerator"));
	IsInteractGenerator = false;
}

void ADBD_Player::Parkour()
{
	if (not IsReachWindows) return;

	UE_LOG(LogTemp, Warning, TEXT("Parkour"));
	PlayAnimMontage(ParkourMontage, 1.f);
	IsReachWindows = false;
}

void ADBD_Player::ParkourFinish()
{
	UE_LOG(LogTemp, Warning, TEXT("ParkourFinish"));
	StopAnimMontage(ParkourMontage);
}

void ADBD_Player::ChangePlayerAnimation()
{
	switch (PlayerState)
	{
	case EPlayerState::Death:
		//PlayerState = playerState;
		break;
	case EPlayerState::Hp1:
		break;
	case EPlayerState::Hp2:
		break;
	case EPlayerState::Hp3:
		break;
	case EPlayerState::Piggyback:
		break;
	case EPlayerState::Hang:
		break;
	default:
		break;
	}
}

void ADBD_Player::ChangePlayerState(EPlayerState playerState)
{
	PlayerState = playerState;
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

	PlayerState = (EPlayerState)Health;
	// 3 : WalkSpeed 226, RunSpeed : 400
	// crouch : WalkSpeed 113
	// 2 : WalkSpeed 226, RunSpeed : 400
	// 1 : WalkSpeed 70
	// 피격시 2초 스피드 : 600

	//if (PlayerState == EPlayerState::Hp3)
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = 226.0f;
	//}
	//else if (PlayerState == EPlayerState::Hp2)
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = 226.0f;
	//}
	//else if (PlayerState == EPlayerState::Hp1)
	//{
	//	GetCharacterMovement()->MaxWalkSpeed = 70.0f;
	//}
	UpdateSpeed();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health : %d"), Health));
}

void ADBD_Player::UpdateSpeed()
{
	switch (PlayerState)
	{
	case EPlayerState::Hp1:
		GetCharacterMovement()->MaxWalkSpeed = 70.0f;
		break;
	case EPlayerState::Hp2:
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
		break;
	case EPlayerState::Hp3:
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
		break;
	default:
		break;
	}
}
