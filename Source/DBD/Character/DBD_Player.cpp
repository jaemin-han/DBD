// Fill out your copyright notice in the Description page of Project Settings.


#include "DBD_Player.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/SphereComponent.h"

#include "Gimmick/DBD_Interface_Gimmick.h"
#include "Components/CapsuleComponent.h"
#include "UI/InteractionUI.h"
#include "Engine/DecalActor.h"
#include "Gimmick/Decal.h"
#include "Kismet/GameplayStatics.h"

// 아래 두개는 추후 종속성 제거 예정 -> Interface로 전부 가능하도록 변경 예정
#include "Gimmick/Pallet.h"
#include "Gimmick/Door.h"
#include "Gimmick/Windows.h"
#include "Net/UnrealNetwork.h"

/** 기본 상속 함수*/
// 기본 생성자 함수
ADBD_Player::ADBD_Player()
{
	SearchGimmickSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SearchGimmickSphere"));
	SearchGimmickSphere->SetupAttachment(RootComponent);
}

// 기본 BeginPlay 함수
void ADBD_Player::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	SurvivorState= ESurvivorState::Hp3;

	if (MainUIClass)
	{
		MainUI = Cast<UInteractionUI>(CreateWidget(GetWorld(), MainUIClass));
		if (MainUI)
		{
			MainUI->AddToViewport();
			UE_LOG(LogTemp, Log, TEXT("MainUI Create Success"));

			MainUI->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// Interation 함수를 0.2초마다 호출
	//FTimerHandle interactionTimer;
	//GetWorld()->GetTimerManager().SetTimer(interactionTimer, this, &ADBD_Player::Interaction, 0.2f, true);
}

// 기본 Tick 함수
void ADBD_Player::Tick(float DeltaTime)
{
	if (!HasAuthority()) return;

	Super::Tick(DeltaTime);

	Interaction();
	GetNearPallet();

	// 살인자에서 추가해줘야하는 부분
	//if (bIsParkour)
	//{
	//	FMoveAlongQuadraticBezier(DeltaTime);
	//	//MoveAlongQuadraticBezier(DeltaTime);
	//}
	
	if (Gimmick)
	{
		VisibleMainUI(true, Gimmick->GetGimmickName(), Gimmick->GetInteractKey());
	}
	else
	{
		VisibleMainUI(false, TEXT(""), TEXT(""));
	}
}

// 변수를 서버 연동해주는 함수
void ADBD_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADBD_Player, NearPallet);
	DOREPLIFETIME(ADBD_Player, SacrificeTime);
}

// Input 설정 함수
void ADBD_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlusHpAction, ETriggerEvent::Started, this, &ADBD_Player::PlusHp);
		EnhancedInputComponent->BindAction(MinusHpAction, ETriggerEvent::Started, this, &ADBD_Player::MinusHp);

		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Triggered, this, &ADBD_Player::ExitDoor);

		// 추후 하나의 함수내에서 구분하여 처리할 예정
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::DropdownPallet);
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::GeneratorSkillCheck);
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Completed, this, &ADBD_Player::ReleasedGeneratorSkillCheck);

		EnhancedInputComponent->BindAction(GeneratorAction, ETriggerEvent::Started, this, &ADBD_Player::PushInteractGenerator);
		EnhancedInputComponent->BindAction(GeneratorAction, ETriggerEvent::Completed, this, &ADBD_Player::NonPushInteractGenerator);


		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ADBD_Player::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ADBD_Player::RunStop);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ADBD_Player::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ADBD_Player::CrouchStop);
	}
}
/** 기본 상속 함수*/



/** 임시 변수*/
void ADBD_Player::PlusHp()
{
	UpdateHP(-1);
}
void ADBD_Player::MinusHp()
{
	UpdateHP(1);
}
/** 임시 변수*/







/** Input 함수 - Multicast로 모든 클라이언트가 동일하게 볼수 있도록 해줘야 하는 부분들?? */
// 달리기 적용 함수 (적용O)
void ADBD_Player::Run()
{
	ServerRPC_Run();
	//// Health가 1이면 리턴
	//if (Health == 1) return;
	//
	//IsRunning = true;
	//GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	//
	//SpawnDecalTime += GetWorld()->GetDeltaSeconds();
	//
	//if (SpawnDecalTime > 0.2f)
	//{
	//	SpawnDecal();
	//	SpawnDecalTime = 0.0f;
	//}
}
// 달리기 적용 서버 함수 (적용O)
void ADBD_Player::ServerRPC_Run_Implementation()
{
	MulticastRPC_Run();
}
// 달리기 적용 멀티캐스트 함수 (적용O)
void ADBD_Player::MulticastRPC_Run_Implementation()
{
	// Health가 1이면 리턴
	if (Health == 1) return;

	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	SpawnDecalTime += GetWorld()->GetDeltaSeconds();

	if (SpawnDecalTime > 0.2f)
	{
		SpawnDecal();
		SpawnDecalTime = 0.0f;
	}
}


// 달리기 멈춤 적용 함수 (적용O)
void ADBD_Player::RunStop()
{

	ServerRPC_RunStop();

	//// Health가 1이면 리턴
	//if (Health == 1) return;
	//
	//IsRunning = false;
	//SpawnDecalTime = 0.0f;
	//GetCharacterMovement()->MaxWalkSpeed = 226.0f;
}
// 달리기 멈춤 적용 서버 함수 (적용O)
void ADBD_Player::ServerRPC_RunStop_Implementation()
{
	MulticastRPC_RunStop();
}
// 달리기 멈춤 적용 멀티캐스트 함수 (적용O)
void ADBD_Player::MulticastRPC_RunStop_Implementation()
{
	// Health가 1이면 리턴
	if (Health == 1) return;

	IsRunning = false;
	SpawnDecalTime = 0.0f;
	GetCharacterMovement()->MaxWalkSpeed = 226.0f;
}

// 앉기 적용 함수 (적용O)
void ADBD_Player::Crouch()
{
	//// Health가 3일때만 작동
	//if (Health != 3) return;
	//
	//IsCrouching = true;
	//GetCharacterMovement()->MaxWalkSpeed = 113.0f;

	ServerRPC_Crouch();
}
// 앉기 적용 서버 함수 (적용O)
void ADBD_Player::ServerRPC_Crouch_Implementation()
{
	MulticastRPC_Crouch();
}
// 앉기 적용 멀티캐스트 함수 (적용O)
void ADBD_Player::MulticastRPC_Crouch_Implementation()
{
	// Health가 3일때만 작동
	if (Health != 3) return;

	IsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = 113.0f;
}


// 앉기 멈춤 적용 함수 (적용O)
void ADBD_Player::CrouchStop()
{
	//// Health가 3일때만 작동
	//if (Health != 3) return;
	//
	//IsCrouching = false;
	//GetCharacterMovement()->MaxWalkSpeed = 226.0f;

	ServerRPC_CrouchStop();
}
// 앉기 멈춤 적용 서버 함수 (적용O)
void ADBD_Player::ServerRPC_CrouchStop_Implementation()
{
	MulticastRPC_CrouchStop();
}
// 앉기 멈춤 적용 멀티캐스트 함수 (적용O)
void ADBD_Player::MulticastRPC_CrouchStop_Implementation()
{
	// Health가 3일때만 작동
	if (Health != 3) return;

	IsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = 226.0f;
}


//// 파쿠르 적용 함수 (적용 X)
//void ADBD_Player::Parkour()
//{
//	//if (not IsReachWindows) return;
//
//	//TArray<AActor*> AllActors;
//	//TArray<AWindows*> Windows;
//	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWindows::StaticClass(), AllActors);
//	//for (AActor* actor : AllActors)
//	//{
//	//	if (AWindows* win = Cast<AWindows>(actor))
//	//	{
//	//		Windows.Add(win);
//	//	}
//	//}
//	////Window = Cast<AWindows>(Windows[0]);
//	//
//	//float closestDist = std::numeric_limits<float>::max();
//	//AWindows* closestWindow = nullptr;
//	//float dist = 0.0f;
//	//for (AWindows* window : Windows)
//	//{
//	//	dist = FVector::Distance(GetActorLocation(), window->GetActorLocation());
//	//	UE_LOG(LogTemp, Error, TEXT("Distance : %f"), dist);
//	//	if (dist < closestDist)
//	//	{
//	//		// 최단 거리 갱신
//	//		closestDist = dist;
//	//		closestWindow = window;
//	//	}
//	//}
//	//
//	//// 캐릭터와 Window의 길이구하기
//	//if(closestDist > 100.0f) return; // 너무 멀면 파쿠르 하지 않기
//	//
//	//// 2차 베지에 곡선 계산
//	////P0 = GetActorLocation();
//	////P1 = P0 + GetActorUpVector() * 100.0f;
//	////P2 = P1 + GetActorForwardVector() * dist;
//	//
//	//// 3차 베지에 곡선 계산
//	//P0 = GetActorLocation();
//	//P1 = P0 + GetActorUpVector() * 120.0f;
//	//P2 = P1 + GetActorForwardVector() * closestDist * 2;
//	//P3 = P0 + GetActorForwardVector() * closestDist * 2;
//
//	//UE_LOG(LogTemp, Warning, TEXT("Parkour"));
//	//PlayAnimMontage(StateMontage, 1.f, TEXT("Parkour"));
//	//IsReachWindows = false;
//	//IsParkour = true;
//
//	ServerRPC_Parkour();
//}
//// 파쿠르 적용 서버 함수 (적용 X)
//void ADBD_Player::ServerRPC_Parkour_Implementation()
//{
//	MulticastRPC_Parkour();
//}
//// 파쿠르 적용 멀티캐스트 함수 (적용 X)
//void ADBD_Player::MulticastRPC_Parkour_Implementation()
//{
//	if (not IsReachWindows) return;
//
//	UE_LOG(LogTemp, Warning, TEXT("Parkour"));
//	PlayAnimMontage(StateMontage, 1.f, TEXT("Parkour"));
//	IsReachWindows = false;
//	IsParkour = true;
//}
//
//
//// 파쿠르 종료 함수 (적용 X)
//void ADBD_Player::ParkourFinish()
//{
//	//UE_LOG(LogTemp, Warning, TEXT("ParkourFinish"));
//	//StopAnimMontage(StateMontage);
//	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//	ServerRPC_ParkourFinish();
//}
//// 파쿠르 종료 서버 함수 (적용 X)
//void ADBD_Player::ServerRPC_ParkourFinish_Implementation()
//{
//	MulticastRPC_ParkourFinish();
//}
//// 파쿠르 종료 멀티캐스트 함수 (적용 X)
//void ADBD_Player::MulticastRPC_ParkourFinish_Implementation()
//{
//	UE_LOG(LogTemp, Warning, TEXT("ParkourFinish"));
//	StopAnimMontage(StateMontage);
//	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//}


// 발전기와 상호작용을 위한 Input 함수 - 발전기와 상호작용 On (발전기 게이지 시작)
void ADBD_Player::PushInteractGenerator()
{
	if (not IsFindGenerator) return;

	IsInteractGenerator = true;
	GetCharacterMovement()->DisableMovement();
}
// 발전기와 상호작용을 위한 Input 함수 - 발전기와 상호작용 Off (발전기 게이지 종료)
void ADBD_Player::NonPushInteractGenerator()
{
	//if (not IsReachGenerator) return;
	//UE_LOG(LogTemp, Warning, TEXT("NonPushInteractGenerator"));
	IsInteractGenerator = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}
// 판자와 상호작용을 위한 Input 함수 - 판자와 상호작용 On (판자 내리기)
void ADBD_Player::DropdownPallet()
{
	if (NearPallet)
	{
		NearPallet->Interaction(this);
	}
}
// 탈출구와 상호작용을 위한 Input 함수 - 탈출구와 상호작용 On (탈출구 게이지 시작)
void ADBD_Player::ExitDoor()
{
	//UE_LOG(LogTemp, Log, TEXT("ExitDoor"));
	//UE_LOG(LogTemp, Log, TEXT("IsOverlapDoor : %d"), IsOverlapDoor);
	if (IsOverlapDoor)
	{
		Door->Interaction();
	}
}

void ADBD_Player::GeneratorSkillCheck()
{
	if (IsSkillCheckZone)
	{
		UE_LOG(LogTemp, Error, TEXT("GeneratorSkillCheck"));
		IsSpaceBar = true;
	}
}

void ADBD_Player::ReleasedGeneratorSkillCheck()
{
	IsSkillCheckZone = false;
	IsSpaceBar = false;
}
/** Input 함수 */




/** 상호작용 함수 - 각각의 클라이언트에서만 할수있게 대신에 변수값은 모든 클라이언트에 연동이 되야하는?*/
// 라인트레이스를 활용한 상호작용 함수
void ADBD_Player::Interaction()
{
	//ServerRPC_Interaction();
	
	FVector startPos = GetActorLocation();
	FVector endPos = startPos + GetActorForwardVector() * 100.0f;

	FHitResult hitResult;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, collisionParams);
	if (bHit)
	{
		//if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(hitResult.GetActor()))
		if (hitResult.GetActor() and hitResult.GetActor()->GetClass()->ImplementsInterface(UDBD_Interface_Gimmick::StaticClass()))
		{
			NearGimmick = hitResult.GetActor();
			// HitActor가 Generator라면
			// 이후 변경 (각각 NameOrLabel로 찾는 거를 어떻게 더 효율적으로 변경할수있을지 생각)
			//if (hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator")
			//	or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator2")
			//	or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator3")
			//	or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator4"))
			if (NearGimmick->GetGimmickName() == TEXT("Generator"))
			{
				//UE_LOG(LogTemp, Warning, TEXT(" Generator"));
				IsFindGenerator = true;
				//Gimmick = gimmick;

				if (IsInteractGenerator)
				{
					NearGimmick->Interaction(); // 게이지 UI 생성 함수
					IsSkillCheckZone = true;
				}
				else
				{
					NearGimmick->FailedInteraction(); // 게이지 UI 제거 함수
					IsSpaceBar = false;
				}
			}
			// HitActor가 Windows라면
			else if (NearGimmick->GetGimmickName() == TEXT("Windows"))
			{
				UE_LOG(LogTemp, Warning, TEXT(" Windows"));

				// 이부분은 추후에 Character클래스에서 호출해주는 방식으로 변경 가능성 있음
				// 살인자에 추가할 부분
				bIsSearchWindows = true;
				//SetBezierPoint();

				// 서버에게 Point값을 전달
				//ServerRPC_SetBezierPoint();
				Server_ReportBezierPoints();

				//ClientRPC_SetBezierPoint(gimmick);


				//Gimmick = gimmick;
			}
		}
	}
	else
	{
		IsFindGenerator = false;
		bIsSearchWindows = false;
		IsInteractGenerator = false;
		if (not IsOverlapDoor) Gimmick = nullptr;
	}

	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);
}
void ADBD_Player::ServerRPC_Interaction_Implementation()
{
	//MulticastRPC_Interaction();
	ClientRPC_Interaction();
}
void ADBD_Player::MulticastRPC_Interaction_Implementation()
{
	//FVector startPos = GetActorLocation();
	//FVector endPos = startPos + GetActorForwardVector() * 100.0f;
	//
	//FHitResult hitResult;
	//FCollisionQueryParams collisionParams;
	//collisionParams.AddIgnoredActor(this);
	//
	//bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, collisionParams);
	//if (bHit)
	//{
	//	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(hitResult.GetActor()))
	//	{
	//		// HitActor가 Generator라면
			// 이후 변경 (각각 NameOrLabel로 찾는 거를 어떻게 더 효율적으로 변경할수있을지 생각)
	//		if (hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator")
	//			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator2")
	//			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator3")
	//			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator4"))
	//		{
	//			//UE_LOG(LogTemp, Warning, TEXT(" Generator"));
	//			IsFindGenerator = true;
	//			Gimmick = gimmick;
	//
	//			if (IsInteractGenerator)
	//			{
	//				gimmick->Interaction(); // 게이지 UI 생성 함수
	//				IsSkillCheckZone = true;
	//			}
	//			else
	//			{
	//				gimmick->FailedInteraction(); // 게이지 UI 제거 함수
	//				IsSpaceBar = false;
	//			}
	//		}
	//		// HitActor가 Windows라면
	//		else if (gimmick->GetGimmickName() == TEXT("Windows"))
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT(" Windows"));
	//
	//			// 이부분은 추후에 Character클래스에서 호출해주는 방식으로 변경 가능성 있음
				// 살인자에 추가할 부분
	//			bIsSearchWindows = true;
	//			SetBezierPoint(gimmick);
	//			//ClientRPC_SetBezierPoint(gimmick);
	//
	//
	//			Gimmick = gimmick;
	//		}
	//		// HitActor가 Board라면
			// HitActor가 출구라면
	//	}
	//}
	//else
	//{
	//	IsFindGenerator = false;
	//	bIsSearchWindows = true;
	//	IsInteractGenerator = false;
	//	if (not IsOverlapDoor) Gimmick = nullptr;
	//}
	//
	//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);
}
void ADBD_Player::ClientRPC_Interaction_Implementation()
{
	//FVector startPos = GetActorLocation();
	//FVector endPos = startPos + GetActorForwardVector() * 100.0f;
	//
	//FHitResult hitResult;
	//FCollisionQueryParams collisionParams;
	//collisionParams.AddIgnoredActor(this);
	//
	//bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, collisionParams);
	//if (bHit)
	//{
	//	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(hitResult.GetActor()))
	//	{
	//		// HitActor가 Generator라면
			// 이후 변경 (각각 NameOrLabel로 찾는 거를 어떻게 더 효율적으로 변경할수있을지 생각)
	//		if (hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator")
	//			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator2")
	//			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator3")
	//			or hitResult.GetActor()->GetActorNameOrLabel() == TEXT("BP_Generator4"))
	//		{
	//			//UE_LOG(LogTemp, Warning, TEXT(" Generator"));
	//			IsFindGenerator = true;
	//			Gimmick = gimmick;
	//
	//			if (IsInteractGenerator)
	//			{
	//				gimmick->Interaction(); // 게이지 UI 생성 함수
	//				IsSkillCheckZone = true;
	//			}
	//			else
	//			{
	//				gimmick->FailedInteraction(); // 게이지 UI 제거 함수
	//				IsSpaceBar = false;
	//			}
	//		}
	//		// HitActor가 Windows라면
	//		else if (gimmick->GetGimmickName() == TEXT("Windows"))
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT(" Windows"));
	//
	//			// 이부분은 추후에 Character클래스에서 호출해주는 방식으로 변경 가능성 있음
				// 살인자에 추가할 부분
	//			bIsSearchWindows = true;
	//			SetBezierPoint(gimmick);
	//			//ClientRPC_SetBezierPoint(gimmick);
	//
	//
	//			Gimmick = gimmick;
	//		}
	//		// HitActor가 Board라면
			// HitActor가 출구라면
	//	}
	//}
	//else
	//{
	//	IsFindGenerator = false;
	//	bIsSearchWindows = true;
	//	IsInteractGenerator = false;
	//	if (not IsOverlapDoor) Gimmick = nullptr;
	//}
	//
	//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);
}
// 가까운 판자를 가져오기 위한 함수
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
	// NewNearPallet 이 유효하면 owner를 나로 설정
	// NewNearPallet 이 null 이면 기존 NearPallet의 owner를 null로 설정
	if (NewNearPallet)
	{
		NewNearPallet->SetOwner(this);
	}
	else
	{
		if (NearPallet)
		{
			NearPallet->SetOwner(nullptr);
		}
	}
	
	NearPallet = NewNearPallet;
		

	// debug NearPallet
	FString DebugString = NearPallet ? NearPallet->GetName() : TEXT("None");
	GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Red, DebugString);

	if (NearPallet)
	{
		Gimmick = NearPallet;
	}
}

void ADBD_Player::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(OtherActor))
	{
		//UE_LOG(LogTemp, Log, TEXT("Gimmick BeginOverlap"));
		// 만약 OtherActor가 Door라면
		if (OtherActor->GetActorNameOrLabel() == TEXT("BP_Door2") or OtherActor->GetActorNameOrLabel() == TEXT("BP_Door"))
		{
			IsOverlapDoor = true;
			Door = Cast<ADoor>(gimmick);
			Gimmick = gimmick;
		}
	}
}

void ADBD_Player::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(OtherActor))
	{
		//UE_LOG(LogTemp, Log, TEXT("Gimmick EndOverlap"));
		// 만약 OtherActor가 Door라면
		if (OtherActor->GetActorNameOrLabel() == TEXT("BP_Door2") or OtherActor->GetActorNameOrLabel() == TEXT("BP_Door"))
		{
			IsOverlapDoor = false;
			IsInteractDoor = false;
			gimmick->FailedInteraction();
			Door = nullptr;
			//Gimmick = nullptr;
		}
	}
}
/** 상호작용 함수*/


void ADBD_Player::SpawnDecal()
{
	ADecal* decal = GetWorld()->SpawnActor<ADecal>(DecalFactory);
	// 큐브의 위치를 나의 앞방향으로 100만큼 떨어진 위치에 놓자.	
	//FVector loc = GetActorLocation() + FVector(0, 100, -GetActorLocation().Z);
	FVector loc = GetActorLocation() + GetActorForwardVector() * -110 + GetActorUpVector() * -GetActorLocation().Z;
	decal->SetActorLocation(loc);
	// 큐브의 회전값을 나의 회전값으로 설정
	//FRotator rot = GetActorRotation() + FRotator(-90, GetActorRotation().Yaw, GetActorRotation().Roll);
	FRotator rot = GetActorRotation() + FRotator(-90, FMath::RandRange(0.0f, 360.0f), GetActorRotation().Roll);
	decal->SetActorRotation(rot);
	decal->SetActorScale3D(FVector(0.3f));

	ADecal* decalLR = GetWorld()->SpawnActor<ADecal>(DecalFactory);
	// 큐브의 위치를 나의 앞방향으로 100만큼 떨어진 위치에 놓자.	
	//FVector locL = GetActorLocation() + FVector(0, 60, -30);
	FVector locL = GetActorLocation() + GetActorForwardVector() * -110 + GetActorUpVector() * -30;
	decalLR->SetActorLocation(locL);
	// 큐브의 회전값을 나의 회전값으로 설정
	FRotator rotL = GetActorRotation() + FRotator(GetActorRotation().Pitch, 90, FMath::RandRange(0.0f, 360.0f));
	decalLR->SetActorRotation(rotL);
	decalLR->SetActorScale3D(FVector(0.80f, 0.3f, 0.26f));
}
void ADBD_Player::VisibleMainUI(bool IsVisible, FString Name, FString Key)
{
	if (MainUI)
	{
		MainUI->SetVisibility(IsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		MainUI->SetGimmickName(Name);
		MainUI->SetInteractKey(Key);
	}
}

// 움직임키입력에 따른 상태 및 스피드 변경 함수
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
// 체력에 따른 상태 변경 함수
void ADBD_Player::UpdateHP(int32 Value)
{
	MulticastRPC_UpdateHP(Value);

}

void ADBD_Player::ServerRPC_UpdateHP_Implementation(int32 Damage)
{
	UpdateHP(Damage);
}

void ADBD_Player::MulticastRPC_UpdateHP_Implementation(int32 Value)
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

// 상태에 따른 설정 변경 함수
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
void ADBD_Player::ChangeSurvivorState(ESurvivorState survivorState)
{
	SurvivorState = survivorState;
	ChangePlayerAnimation();
}