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
#include "Gimmick/Hanger.h"
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

	Super::Tick(DeltaTime);
	if (!HasAuthority()) return;

	if (IsLocallyControlled())
	{
		if (NearGimmick && NearGimmick->GetGimmickName() != "Pallet")
		{
			VisibleMainUI(true, NearGimmick->GetGimmickName(), NearGimmick->GetInteractKey());
		}
		// 판자가 있고, 내려가있지 않다면
		else if (NearPallet && !NearPallet->bIsFallen)
		{
			VisibleMainUI(true, NearPallet->GetGimmickName(), NearPallet->GetInteractKey());
		}
		else
		{
			VisibleMainUI(false, TEXT(""), TEXT(""));
		}
	}
	

	

	Interaction();
	GetNearPallet();
	//GetNearSurvivor();
	PrintDebug();

	// 살인자에서 추가해줘야하는 부분
	//if (bIsParkour)
	//{
	//	FMoveAlongQuadraticBezier(DeltaTime);
	//	//MoveAlongQuadraticBezier(DeltaTime);
	//}
	
	if (IsClickedRaiseSurvivorButton)
	{
		Server_RaiseFallenSurvivor();
		Server_ActivatedGaugeUI();
	}


	if (NearGimmick)
	{
		Client_VisibleInteractUI(IsInteractGenerator, bIsParkour);
	}
	else
	{
		Client_HiddenInteractUI(); 
	}
}

// 변수를 서버 연동해주는 함수
void ADBD_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADBD_Player, NearPallet);
	DOREPLIFETIME(ADBD_Player, OtherSurvivor);
}

// Input 설정 함수
void ADBD_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PlusHpAction, ETriggerEvent::Started, this, &ADBD_Player::PlusHp);
		EnhancedInputComponent->BindAction(MinusHpAction, ETriggerEvent::Started, this, &ADBD_Player::MinusHp);

		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Triggered, this, &ADBD_Player::Server_ExitDoor);

		// 추후 하나의 함수내에서 구분하여 처리할 예정
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::DropdownPallet);
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Started, this, &ADBD_Player::GeneratorSkillCheck);
		EnhancedInputComponent->BindAction(ParkourAction, ETriggerEvent::Completed, this, &ADBD_Player::ReleasedGeneratorSkillCheck);

		EnhancedInputComponent->BindAction(GeneratorAction, ETriggerEvent::Started, this, &ADBD_Player::PushInteractGenerator);
		EnhancedInputComponent->BindAction(GeneratorAction, ETriggerEvent::Completed, this, &ADBD_Player::NonPushInteractGenerator);
		EnhancedInputComponent->BindAction(RaiseUpAction, ETriggerEvent::Started, this, &ADBD_Player::Server_RaiseFallenSurvivor);
		EnhancedInputComponent->BindAction(RaiseUpAction, ETriggerEvent::Completed, this, &ADBD_Player::NonRaiseFallenSurvivor);


		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ADBD_Player::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ADBD_Player::RunStop);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ADBD_Player::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ADBD_Player::CrouchStop);

		EnhancedInputComponent->BindAction(RescueAction, ETriggerEvent::Started, this, &ADBD_Player::ServerRPC_Rescue);
	}
}
/** 기본 상속 함수*/



/** 임시 변수*/
void ADBD_Player::PlusHp()
{
	ServerRPC_UpdateHP(-1);
}
void ADBD_Player::MinusHp()
{
	ServerRPC_UpdateHP(1);
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

void ADBD_Player::Server_ExitDoor_Implementation()
{
	ExitDoor();
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

void ADBD_Player::ServerRPC_Rescue_Implementation()
{
	if (NearGimmick && NearGimmick->GetGimmickName() == TEXT("Hanger"))
	{
		// 지금 Rescue 가능한지 판단하려면 Hanger의 상태를 확인해야함
		auto* Hanger = Cast<AHanger>(NearGimmick.GetObject());
		if (Hanger->GetHangSurvivor())
		{
			MulticastRPC_Rescue();
		}
	}
}

void ADBD_Player::MulticastRPC_Rescue_Implementation()
{
	PlayAnimMontage(RescueMontage);
}

/** Input 함수 */


/** 상호작용 함수 - 각각의 클라이언트에서만 할수있게 대신에 변수값은 모든 클라이언트에 연동이 되야하는?*/
// 라인트레이스를 활용한 상호작용 함수
void ADBD_Player::Interaction()
{
	FVector startPos = GetActorLocation();
	FVector endPos = startPos + GetActorForwardVector() * 100.0f;

	FHitResult hitResult;
	//FCollisionQueryParams collisionParams;
	//collisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility);
	if (bHit)
	{
		if (hitResult.GetActor() and hitResult.GetActor()->GetClass()->ImplementsInterface(UDBD_Interface_Gimmick::StaticClass()))
		{
			if (ADBD_Player* otherPlayer = Cast<ADBD_Player>(hitResult.GetActor()))
			{
				UE_LOG(LogTemp, Warning, TEXT("OtherPlayer"));
				return;
			}


			NearGimmick = hitResult.GetActor();
			//UE_LOG(LogTemp, Warning, TEXT("NearGimmick : %s"), *NearGimmick->GetGimmickName());


			if (NearGimmick->GetGimmickName() == TEXT("Generator"))
			{
				IsFindGenerator = true;

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

				// 이부분은 추후에 Character클래스에서 호출해주는 방식으로 변경 가능성 있음
				// 살인자에 추가할 부분
				bIsFindWindows = true;
			}

			else if (NearGimmick->GetGimmickName() == TEXT("Pallet"))
			{
				TracePallet = Cast<APallet>(NearGimmick.GetObject());
				if (TracePallet)
				{
					if (TracePallet->bIsFallen) // 넘어진 상태라면?
					{
						bIsFindPallet = true;
					}

					//UE_LOG(LogTemp, Warning, TEXT("bIsFindPallet : %d"), bIsFindPallet);
				}

			}
		}
	}
	else
	{
		// todo: 재민 추가
		NearGimmick = nullptr;
		//
		
		IsFindGenerator = false;
		IsInteractGenerator = false;

		bIsFindWindows = false;
		bIsFindPallet = false;
		if (not IsOverlapDoor) NearGimmick = nullptr;
	}

	DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red, false, 1.0f, 0, 1.0f);
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
	// FString DebugString = NearPallet ? NearPallet->GetName() : TEXT("None");
	// GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Red, DebugString);

	if (NearPallet)
	{
		Gimmick = NearPallet;
	}
}

void ADBD_Player::GetNearSurvivor()
{
	// SearchGimmickSphere 와 겹치는 엑터 중, IDBD_Interface_Gimmick 인터페이스를 구현한 엑터를 찾아 NearGimmick 에 할당
	TArray<AActor*> OverlappingActors;
	SearchGimmickSphere->GetOverlappingActors(OverlappingActors);
	ADBD_Player* NewNearSurvivor = nullptr;

	for (AActor* OverlappingActor : OverlappingActors)
	{
		// 오버랩되고있는게 본인이라면
		if (OverlappingActor == this)
		{
			continue;
		}

		NewNearSurvivor = Cast<ADBD_Player>(OverlappingActor);
	}
	if (NewNearSurvivor)
	{
		OtherSurvivor = NewNearSurvivor;
		UE_LOG(LogTemp, Warning, TEXT("[%s] OtherSurvivor : %s"), *GetName(), *OtherSurvivor->GetName());
	}
	else
	{
		OtherSurvivor = nullptr;
		//UE_LOG(LogTemp, Warning, TEXT("[%s] OtherSurvivor is nullptr"), *GetName());
	}
}

void ADBD_Player::NotifyActorBeginOverlap(AActor* OtherActor)
{
	//if(not HasAuthority()) return;

	Super::NotifyActorBeginOverlap(OtherActor);

	ADBD_Player* otherPlayer = Cast<ADBD_Player>(OtherActor);
	if (otherPlayer and OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] OtherPlayer : %s"),*GetName(), *otherPlayer->GetName());
		OtherSurvivor = otherPlayer;
	}

	if (IDBD_Interface_Gimmick* gimmick = Cast<IDBD_Interface_Gimmick>(OtherActor))
	{
		// 만약 OtherActor가 Door라면
		if (gimmick->GetGimmickName() == TEXT("Door"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Door BeginOverlap"));
			IsOverlapDoor = true;
			Door = Cast<ADoor>(gimmick);
			Gimmick = gimmick;
		}
	}
}

void ADBD_Player::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	ADBD_Player* otherPlayer = Cast<ADBD_Player>(OtherActor);
	if (otherPlayer and OtherActor != this)
	{
		OtherSurvivor = nullptr;
		IsClickedRaiseSurvivorButton = false;
		IsFinishRaiseSurvivor = false;
		RaiseSurvivorTimer = 0.0f;
	}


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

void ADBD_Player::RaiseFallenSurvivor(ADBD_Player* otherSurvivor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Local RaiseFallenSurvivor"));
	UE_LOG(LogTemp, Warning, TEXT("<Local> [%s] OtehrPlayer : %s"), *GetName(), *otherSurvivor->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("[%s] OtehrPlayer state : %d"), *GetName(), otherSuvivor->GetSurvivorState());
	//if (otherSuvivor and otherSuvivor->GetSurvivorState() == ESurvivorState::Hp1)
	{
		IsClickedRaiseSurvivorButton = true;
		//Server_ActivatedGaugeUI();
		
		//otherSurvivor->PlusHp();
		if (IsFinishRaiseSurvivor)
		{
			IsFinishRaiseSurvivor = false;
			RaiseSurvivorTimer = 0.0f;
			IsClickedRaiseSurvivorButton = false;
			otherSurvivor->PlusHp();
			//Multi_RaiseFallenSurvivor(otherSurvivor);
		}
	}
}
void ADBD_Player::Server_RaiseFallenSurvivor_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("Server RaiseFallenSurvivor : %s"), *OtherSurvivor->GetName());
	if (OtherSurvivor and OtherSurvivor->GetSurvivorState() == ESurvivorState::Hp1)
	{
		RaiseFallenSurvivor(OtherSurvivor);
	}
}
void ADBD_Player::Multi_RaiseFallenSurvivor_Implementation(ADBD_Player* otherSurvivor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Multi RaiseFallenSurvivor"));
	//UE_LOG(LogTemp, Warning, TEXT("[%s] OtehrPlayer : %s"), *GetName(), *otherSuvivor->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("[%s] OtehrPlayer state : %d"), *GetName(), otherSuvivor->GetSurvivorState());
	//
	UE_LOG(LogTemp, Warning, TEXT("OtherPlayer healing"));
	otherSurvivor->PlusHp();
}

void ADBD_Player::NonRaiseFallenSurvivor()
{
	IsClickedRaiseSurvivorButton = false;
	RaiseSurvivorTimer = 0.0f;
}

void ADBD_Player::ActivatedGaugeUI(float time)
{
	if (not IsClickedRaiseSurvivorButton) return;


	RaiseSurvivorTimer += time;
	UE_LOG(LogTemp, Warning, TEXT("[%s] RaiseSurvivorTimer : %.2f"), *GetName(), RaiseSurvivorTimer);

	if (RaiseSurvivorTimer >= 3.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ActivatedGaugeUI"), *GetName());
		IsFinishRaiseSurvivor = true;
		//NonRaiseFallenSurvivor();
	}
	
	//IsSkillCheckZone = true;
}
void ADBD_Player::Server_ActivatedGaugeUI_Implementation()
{
	ActivatedGaugeUI(GetWorld()->DeltaTimeSeconds);
}
void ADBD_Player::Client_ActivatedGaugeUI_Implementation()
{
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

void ADBD_Player::PrintDebug()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[%s] Health : %d"), *GetName(), Health));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[%s] State : %d"), *GetName(), (int)SurvivorState));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("[%s] State : %d"), *GetName(), (int)SurvivorState));
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

// 안쓰는 함수들?
void ADBD_Player::VisibleInteractUI()
{
	if (NearGimmick)
	{
		if (IsInteractGenerator) return;
		if (bIsParkour) return;

	


		//Client_VisibleInteractUI();
	}
}
void ADBD_Player::Server_VisibleInteractUI_Implementation()
{
	VisibleInteractUI();
}
// 안쓰는 함수들?

void ADBD_Player::Client_VisibleInteractUI_Implementation(bool IsGenerator, bool IsParkour)
{
	if (!IsLocallyControlled()) return;

	if (not NearGimmick)
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("IsInteractGenerator : %d"), IsGenerator);
	//UE_LOG(LogTemp, Warning, TEXT("bIsParkour : %d"), IsParkour);

	if (IsParkour or IsGenerator)
	{
		Client_HiddenInteractUI();
		return;
	}

	//return;
	if (MainUI)
	{
		MainUI->SetVisibility(ESlateVisibility::Visible);
		MainUI->SetGimmickName(NearGimmick->GetGimmickName());
		MainUI->SetInteractKey(NearGimmick->GetInteractKey());
	}
}


// 안쓰는 함수들?
void ADBD_Player::HiddenInteractUI()
{
	if (NearGimmick)
	{
		Client_HiddenInteractUI();
	}
}
void ADBD_Player::Server_HiddenInteractUI_Implementation()
{
	HiddenInteractUI();
}
// 안쓰는 함수들?


void ADBD_Player::Client_HiddenInteractUI_Implementation()
{
	// 왜 이렇게 되는지 이해를 못하겠다..

	if (not NearGimmick)
	{
		//UE_LOG(LogTemp, Error, TEXT("[Client] [%s] NearGimmick is nullptr"), *GetOwner()->GetName());
		if (HasAuthority() and MainUI)
		{
			MainUI->SetVisibility(ESlateVisibility::Hidden);
			MainUI->SetGimmickName(TEXT(""));
			MainUI->SetInteractKey(TEXT(""));
		}

		return;
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("[Client] [%s] HiddenInteractUI"), *GetOwner()->GetName());
		if (MainUI)
		{
			MainUI->SetVisibility(ESlateVisibility::Hidden);
			MainUI->SetGimmickName(TEXT(""));
			MainUI->SetInteractKey(TEXT(""));
		}
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


	UpdateSpeed();
	// 임시
	ChangePlayerAnimation();
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
	
	// Health 와 SurvivorState를 연동
	Health = static_cast<int>(survivorState);
	
	// ChangeSurvivorState 를 호출했을 때도 Speed 변경
	UpdateSpeed();

	ChangePlayerAnimation();
}