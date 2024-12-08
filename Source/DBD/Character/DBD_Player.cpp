﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "DBD_Player.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"

#include "Gimmick/DBD_Interface_Gimmick.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/GameplayStatics.h"
// 아래 두개는 추후 종속성 제거 예정 -> Interface로 전부 가능하도록 변경 예정
#include "Gimmick/Pallet.h"
#include "Gimmick/Door.h"
#include "Gimmick/Windows.h"

ADBD_Player::ADBD_Player()
{
	SearchGimmickSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SearchGimmickSphere"));
	SearchGimmickSphere->SetupAttachment(RootComponent);
}

void ADBD_Player::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	SurvivorState= ESurvivorState::Hp3;
	// Interation 함수를 0.2초마다 호출
	//FTimerHandle interactionTimer;
	//GetWorld()->GetTimerManager().SetTimer(interactionTimer, this, &ADBD_Player::Interaction, 0.2f, true);
}

void ADBD_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Interaction();
	GetNearPallet();

	if (IsParkour)
	{
		MoveAlongQuadraticBezier(DeltaTime);
	}
}

void ADBD_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlusHpAction, ETriggerEvent::Started, this, &ADBD_Player::PlusHp);
		EnhancedInputComponent->BindAction(MinusHpAction, ETriggerEvent::Started, this, &ADBD_Player::MinusHp);

		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Triggered, this, &ADBD_Player::ExitDoor);

		// 추후 하나의 함수내에서 구분하여 처리할 예정
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::Parkour);
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::DropdownPallet);

		EnhancedInputComponent->BindAction(GeneratorAction, ETriggerEvent::Started, this, &ADBD_Player::PushInteractGenerator);
		EnhancedInputComponent->BindAction(GeneratorAction, ETriggerEvent::Completed, this, &ADBD_Player::NonPushInteractGenerator);


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
			// 이후 변경 (각각 NameOrLabel로 찾는 거를 어떻게 더 효율적으로 변경할수있을지 생각)
			if (hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator") 
			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator2") 
			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator3")
			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator4"))
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
				UE_LOG(LogTemp, Warning, TEXT("%s"), *hitResult.GetActor()->GetActorNameOrLabel());
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

	//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);
}

void ADBD_Player::GetNearPallet()
{
	// SearchGimmickSphere 와 겹치는 엑터 중, IDBD_Interface_Gimmick 인터페이스를 구현한 엑터를 찾아 NearGimmick 에 할당
	TArray<AActor*> OverlappingActors;
	SearchGimmickSphere->GetOverlappingActors(OverlappingActors);
	APallet* NewNearPallet = nullptr;
	for (AActor* OverlappingActor : OverlappingActors)
	{
		NewNearPallet = Cast<APallet>(OverlappingActor);
		if (NewNearPallet)
			break;
	}
	NearPallet = NewNearPallet;

	// debug NearPallet
	FString DebugString = NearPallet ? NearPallet->GetName() : TEXT("None");
	GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Red, DebugString);
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
	GetCharacterMovement()->DisableMovement();
}

void ADBD_Player::NonPushInteractGenerator()
{
	//if (not IsReachGenerator) return;
	//UE_LOG(LogTemp, Warning, TEXT("NonPushInteractGenerator"));
	IsInteractGenerator = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ADBD_Player::Parkour()
{
	//if (not IsReachWindows) return;

	TArray<AActor*> AllActors;
	TArray<AWindows*> Windows;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWindows::StaticClass(), AllActors);
	for (AActor* actor : AllActors)
	{
		if (AWindows* win = Cast<AWindows>(actor))
		{
			Windows.Add(win);
		}
	}
	//Window = Cast<AWindows>(Windows[0]);

	float closestDist = std::numeric_limits<float>::max();
	AWindows* closestWindow = nullptr;
	float dist = 0.0f;
	for (AWindows* window : Windows)
	{
		dist = FVector::Distance(GetActorLocation(), window->GetActorLocation());
		UE_LOG(LogTemp, Error, TEXT("Distance : %f"), dist);
		if (dist < closestDist)
		{
			// 최단 거리 갱신
			closestDist = dist;
			closestWindow = window;
		}
	}

	// 캐릭터와 Window의 길이구하기
	if(closestDist > 100.0f) return; // 너무 멀면 파쿠르 하지 않기

	// 2차 베지에 곡선 계산
	//P0 = GetActorLocation();
	//P1 = P0 + GetActorUpVector() * 100.0f;
	//P2 = P1 + GetActorForwardVector() * dist;

	// 3차 베지에 곡선 계산
	P0 = GetActorLocation();
	P1 = P0 + GetActorUpVector() * 120.0f;
	P2 = P1 + GetActorForwardVector() * closestDist * 2;
	P3 = P0 + GetActorForwardVector() * closestDist * 2;

	UE_LOG(LogTemp, Warning, TEXT("Parkour"));
	PlayAnimMontage(StateMontage, 1.f, TEXT("Parkour"));
	IsReachWindows = false;
	IsParkour = true;
}

void ADBD_Player::DropdownPallet()
{
	if (NearPallet)
	{
		NearPallet->Interaction(this);
	}
}

void ADBD_Player::ExitDoor()
{
	UE_LOG(LogTemp, Log, TEXT("ExitDoor"));
	UE_LOG(LogTemp, Log, TEXT("IsOverlapDoor : %d"), IsOverlapDoor);
	if (IsOverlapDoor)
	{
		Door->Interaction();
	}
}

void ADBD_Player::ParkourFinish()
{
	UE_LOG(LogTemp, Warning, TEXT("ParkourFinish"));
	StopAnimMontage(StateMontage);
}

void ADBD_Player::ChangeSurvivorState(ESurvivorState survivorState)
{
	SurvivorState = survivorState;
	ChangePlayerAnimation();
}

void ADBD_Player::ChangePlayerAnimation()
{
	switch (SurvivorState)
	{
	case ESurvivorState::Death:
		//PlayerState = playerState;
		break;
	case ESurvivorState::Hp1:
		IsPiggyback = false;
		IsHang = false;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		break;
	case ESurvivorState::Hp2:
		IsPiggyback = false;
		IsHang = false;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		break;
	case ESurvivorState::Hp3:
		IsPiggyback = false;
		IsHang = false;
		// CharacterMovement 활성화
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		break;
	case ESurvivorState::Piggyback:
		IsPiggyback = true;
		IsHang = false;
		// MovementComponent 비활성화
		GetCharacterMovement()->DisableMovement();
		//PlayAnimMontage(StateMontage, 1.f, TEXT("Piggy"));
		break;
	case ESurvivorState::Hang:
		IsHang = true;
		IsPiggyback = false; 
		GetCharacterMovement()->DisableMovement();
		//PlayAnimMontage(StateMontage, 1.f, TEXT("Hang"));
		break;
	default:
		break;
	}
}

void ADBD_Player::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Gimmick BeginOverlap"));
		// 만약 OtherActor가 Door라면
		if (OtherActor->GetActorNameOrLabel() == TEXT("BP_Door2") or OtherActor->GetActorNameOrLabel() == TEXT("BP_Door"))
		{
			IsOverlapDoor = true;
			Door = Cast<ADoor>(gimmick);
		}
	}
}

void ADBD_Player::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Gimmick EndOverlap"));
		// 만약 OtherActor가 Door라면
		if (OtherActor->GetActorNameOrLabel() == TEXT("BP_Door2") or OtherActor->GetActorNameOrLabel() == TEXT("BP_Door"))
		{
			IsOverlapDoor = false;
			IsInteractDoor = false;
			gimmick->FailedInteraction();
			Door = nullptr;
		}
	}
}

void ADBD_Player::UpdateHP(int32 Value)
{
	// Value : Damage or Heal
	Health -= Value;
	

	if (Health > 5)
	{
		Health = 5;
	}
	else if (Health < 1)
	{
		Health = 1;
	}

	SurvivorState = (ESurvivorState)Health;
	// 3 : WalkSpeed 226, RunSpeed : 400
	// crouch : WalkSpeed 113
	// 2 : WalkSpeed 226, RunSpeed : 400
	// 1 : WalkSpeed 70
	// 피격시 2초 스피드 : 600
	UpdateSpeed();

	// 임시
	ChangePlayerAnimation();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health : %d"), Health));
	//FString string = GetDisplayNameTextByIndex(SurvivorState).toString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("State : %d"), (int)SurvivorState));
}

void ADBD_Player::UpdateSpeed()
{
	switch (SurvivorState)
	{
	case ESurvivorState::Hp1:
		GetCharacterMovement()->MaxWalkSpeed = 70.0f;
		break;
	case ESurvivorState::Hp2:
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
		break;
	case ESurvivorState::Hp3:
		GetCharacterMovement()->MaxWalkSpeed = 226.0f;
		break;
	default:
		break;
	}
}

/**베지에 곡선 활용
* 2차 베지에 곡선
* P0 : 시작점, P1 : 제어점, P2 : 끝점, t : 시간
* B(t) = (1 - t)^2 * P0 + 2 * (1 - t) * t * P1 + t^2 * P2
*/
FVector ADBD_Player::CalculateQuadraticBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2)
{
	float oneMinusT = 1.0f - t;
	FVector result = oneMinusT * oneMinusT * p0 +
					2 * oneMinusT * t * p1 +
					t * t * p2;

	return result;
}
/*
* 3차 베지에 곡선
* P0 : 시작점, P1 : 제어점1, P2 : 제어점2, P3 : 끝점, t : 시간
* B(t) = (1 - t)^3 * P0 + 3 * (1 - t)^2 * t * P1 + 3 * (1 - t) * t^2 * P2 + t^3 * P3
*/
FVector ADBD_Player::CalculateBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2, const FVector& p3)
{
	float oneMinusT = 1.0f - t;
	FVector result = oneMinusT * oneMinusT * oneMinusT * p0 +
					3 * oneMinusT * oneMinusT * t * p1 +
					3 * oneMinusT * t * t * p2 +
					t * t * t * p3;

	return result;
}

void ADBD_Player::MoveAlongQuadraticBezier(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("MoveAlongQuadraticBezier"));
	
	// 한번 선언하면 다시 초기화 되지 않는 변수 선언
	static float time = 0.0f;
	time += DeltaTime;


	// 2차 베지에 곡선 계산
	// float t = FMath::Clamp(time / 0.5f, 0.0f, 1.0f);
	// FVector newPos = CalculateQuadraticBezierPoint(t, P0, P1, P2);

	// 3차 베지에 곡선 계산
	float t = FMath::Clamp(time, 0.0f, 1.0f);
	FVector newPos = CalculateBezierPoint(t, P0, P1, P2, P3);

	SetActorLocation(newPos);
	if (t >= 1.0f)
	{
		time = 0.0f;
		IsParkour = false;
		return;
	}
}
