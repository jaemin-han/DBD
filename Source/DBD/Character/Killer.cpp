// Fill out your copyright notice in the Description page of Project Settings.


#include "Killer.h"

#include "DBD_Player.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/DBDGameState.h"
#include "Gimmick/DBD_Interface_Gimmick.h"
#include "Gimmick/Hanger.h"
#include "Gimmick/Pallet.h"
#include "Net/UnrealNetwork.h"
#include "UI/InteractionUI.h"
#include "GameMode/LobbyGameState.h"
#include "GameMode/PlayGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

#include "GameMode/LobbyGameState.h"
#include "UI/LobbyUI.h"

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

	if (not HasAuthority()) return; 


	// LobbyGameState 가져오기
	ALobbyGameState* lobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (lobbyGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("[Killer] LobbyGameState"));
		ACameraActor* killerCamera = nullptr;
	
		TArray<AActor*> cameraActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), cameraActors);
		for (auto cameraActor : cameraActors)
		{
			ACameraActor* camera = Cast<ACameraActor>(cameraActor);
			if (camera->GetActorLabel().Contains(TEXT("KillerCam")))
			{
				killerCamera = camera;
				break;
			}
		}
	
		if (killerCamera)
		{
			UE_LOG(LogTemp, Error, TEXT("[Killer] KillerCamera"));
			// 카메라를 레벨에 있는 카메마로 전환해주기
			APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			playerController->SetInputMode(FInputModeUIOnly());
			playerController->SetShowMouseCursor(true);
			playerController->SetViewTarget(killerCamera);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Killer] KillerCamera is nullptr"));
		}
	}

	// PlayGameState 가져오기
	ADBDGameState* playGameState = Cast<ADBDGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (playGameState)
	{
		ParkourSpeed = 0.5f;

		// Interaction UI 생성
		if (InteractionUIClass)
		{
			InteractionUI = Cast<UInteractionUI>(CreateWidget(GetWorld(), InteractionUIClass));
			if (InteractionUI)
			{
				InteractionUI->AddToViewport();
				UE_LOG(LogTemp, Log, TEXT("InteractionUI Create Success"));

				InteractionUI->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("InteractionUI Create Failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InteractionUIClass is nullptr"));
		}

		// 파쿠르라 완료되지 않아도 애니메이션이 끝나면 FinishParkourFunc 함수를 호출
		// 이렇게 하면 넘는 중에 공격 해도 괜찮음
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &ADBDCharacter::OnParkourMontageEnded);
		}

		if (IsLocallyControlled())
		{
			DBDGameState = Cast<ADBDGameState>(GetWorld()->GetGameState());
			if (DBDGameState)
			{
				DBDGameState->InitArrays();
				DBDGameState->SetGeneratorCustomDepth(true);
			}
		}


		ACameraActor* camera = Cast<ACameraActor>(GetFollowCamera());

		if (camera)
		{
			// 카메라를 레벨에 있는 카메마로 전환해주기
			APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			playerController->SetInputMode(FInputModeGameOnly());
			playerController->SetShowMouseCursor(false);
			playerController->SetViewTarget(camera);
		}
	}


	
	// SetActorTickEnabled(false);
}

void AKiller::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AKiller::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);
	//if(not HasAuthority()) return;


	// GameState 가져오고
	ALobbyGameState* gameState = Cast<ALobbyGameState>(GetWorld()->GetGameState());
	// GameUI 가져와서 PlayerStateUI 하나 만들어주세요
	if (gameState)
	{
		gameState->GetLobbyUI()->AddKillerCountUI(NewPlayerState);
	}
}

void AKiller::Attack()
{
	if (bStunned)
		return;

	// 파쿠르 중이 아닐 때 && 공격 중이 아닐 때에만 공격 가능
	if (!bIsParkour && !bIsAttacking)
	{
		MulticastRPC_Attack();
	}
}

void AKiller::ServerRPC_Attack_Implementation()
{
	Attack();
}

void AKiller::MulticastRPC_Attack_Implementation()
{
	bIsAttacking = true;
	PlayAnimMontage(KillerMontage, 1.0f, FName("Attack"));
}

// Called every frame
void AKiller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// InteractionUI 표시
	if (IsLocallyControlled() && InteractionUI)
	{
		ShowInteractionUI();
	}

	if (!HasAuthority())
		return;
	GetNearSurvivor();
	GetNearGimmick();
	Debug();
}

// Called to bind functionality to input
void AKiller::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	// bind attack action
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AKiller::ServerRPC_Attack);

	// InteractionAction 에 대한 바인딩 추가
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &AKiller::Interact);
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this,
	                                   &AKiller::ServerRPC_CarrySurvivor);
	EnhancedInputComponent->BindAction(DropDownSurvivorAction, ETriggerEvent::Started, this,
	                                   &AKiller::ServerRPC_DropDownSurvivor);
}

void AKiller::GetNearSurvivor()
{
	// SearchGimmickSphere 와 겹치는 엑터 중, IDBD_Interface_Gimmick 인터페이스를 구현한 엑터를 찾아 NearGimmick 에 할당
	TArray<AActor*> OverlappingActors;
	SearchGimmickSphere->GetOverlappingActors(OverlappingActors);

	ADBD_Player* NewNearSurvivor = nullptr;
	float MinDistance = std::numeric_limits<float>::max();

	for (AActor* OverlappingActor : OverlappingActors)
	{
		auto* Survivor = Cast<ADBD_Player>(OverlappingActor);
		if (Survivor)
		{
			float Distance = FVector::DistSquared(GetActorLocation(), Survivor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				NewNearSurvivor = Survivor;
			}
		}
	}

	if (NearSurvivor != NewNearSurvivor)
	{
		MulticastRPC_SetNearSurvivor(NewNearSurvivor);
	}
}

void AKiller::MulticastRPC_SetNearSurvivor_Implementation(ADBD_Player* NewNearSurvivor)
{
	NearSurvivor = NewNearSurvivor;
}

void AKiller::GetNearGimmick()
{
	// line trace 를 통해 킬러의 전방에 있는 기믹을 찾아 NearGimmick 에 할당
	FVector Start = GetActorLocation();
	Start.Z -= 90.0f;
	FVector End = Start + GetActorForwardVector() * 100.0f;
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionQueryParams);
	// debug line trace
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0, 1.0f);
	if (HitResult.GetActor() && HitResult.GetActor()->GetClass()->ImplementsInterface(
		UDBD_Interface_Gimmick::StaticClass()))
	{
		NearGimmick = HitResult.GetActor();

		// todo: 부모에서 bIsSearchWindows 변수를 사용하기 위해 할당, 이후에 부모 클래스로 모두 옮기자
		if (NearGimmick->GetGimmickName() == "Windows")
		{
			bIsFindWindows = true;
		}
	}
	else
	{
		NearGimmick = nullptr;

		bIsFindWindows = false;
	}
}

void AKiller::ShowInteractionUI()
{
	// 기믹이 유효한 경우
	if (NearGimmick)
	{
		// 기믹의 이름이 Pallet 인 경우
		if (NearGimmick->GetGimmickName() == "Pallet")
		{
			// 판자가 쓰러져 있는 경우
			auto* Pallet = Cast<APallet>(NearGimmick.GetObject());
			if (Pallet->bIsFallen)
			{
				SetInteractionUI(true, Pallet->GetGimmickName(), Pallet->GetInteractKey());
			}
		}
		// 기믹의 이름이 Hanger 인 경우
		else if (NearGimmick->GetGimmickName() == "Hanger")
		{
			// 지금 들고 있는 생존자가 있는 경우
			if (CarriedSurvivor)
			{
				SetInteractionUI(true, NearGimmick->GetGimmickName(), NearGimmick->GetInteractKey());
			}
		}
		// 기믹의 이름이 Window 인 경우
		else if (NearGimmick->GetGimmickName() == "Window")
		{
			SetInteractionUI(true, NearGimmick->GetGimmickName(), NearGimmick->GetInteractKey());
		}
	}
	// 기믹이 유요하지 않고, 근처에 빈사 상태 생존자가 있는 경우
	else if (NearSurvivor)
	{
		// 빈사 상태인가요?
		if (NearSurvivor->GetSurvivorState() == ESurvivorState::Hp1)
		{
			SetInteractionUI(true, TEXT("Carry Survivor"), TEXT("Space"));
		}
		else
		{
			SetInteractionUI(false, TEXT(""), TEXT(""));
		}
	}
	else
	{
		SetInteractionUI(false, TEXT(""), TEXT(""));
	}
}

void AKiller::SetInteractionUI(bool IsVisible, FString Name, FString Key)
{
	if (InteractionUI)
	{
		InteractionUI->SetVisibility(IsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		InteractionUI->SetGimmickName(Name);
		InteractionUI->SetInteractKey(Key);
	}
}

void AKiller::Debug()
{
	// NearGimmick 에 무엇이 할당되어 있는지 확인 후 스크린에 출력
	FString GimmickName = FString::Printf(
		TEXT("Gimmick: %s"), NearGimmick.GetObject() ? *NearGimmick.GetObject()->GetName() : TEXT("None"));
	GEngine->AddOnScreenDebugMessage(123213, 0.0f, FColor::Red, GimmickName);
	// NearSurvivor 에 무엇이 할당되어 있는지 확인 후 스크린에 출력
	FString SurvivorName =
		FString::Printf(TEXT("Survivor: %s"), NearSurvivor ? *NearSurvivor->GetName() : TEXT("None"));
	GEngine->AddOnScreenDebugMessage(123214, 0.0f, FColor::Red, SurvivorName);
}

void AKiller::DestroyPallet_Implementation()
{
	MulticastRPC_DestroyPallet();
}

void AKiller::MulticastRPC_DestroyPallet_Implementation()
{
	PlayAnimMontage(KillerMontage, 0.5f, FName("DestroyPallet"));
}

void AKiller::Stun_Implementation()
{
	// Stun 몽타주 실행
	bStunned = true;
	PlayAnimMontage(KillerMontage, 2.0f, FName("Stun"));
}

void AKiller::Interact()
{
	// NearGimmick 이 유효한 경우 Interaction 함수 호출
	if (NearGimmick.GetObject() && !bStunned && !bIsAttacking)
	{
		NearGimmick->Interaction(this);
	}
}

void AKiller::CarrySurvivor()
{
	if (IsAttacking() || bStunned)
	{
		return;
	}
	// 근처에 생존자가 있고, 해당 생존자의 체력이 1인 경우 생존자를 옮길 수 있음
	if (NearSurvivor && NearSurvivor->GetHealth() == 1 && CarriedSurvivor == nullptr)
	{
		MulticastRPC_CarrySurvivor();
	}
}

void AKiller::ServerRPC_CarrySurvivor_Implementation()
{
	CarrySurvivor();
}

void AKiller::MulticastRPC_CarrySurvivor_Implementation()
{
	// post process
	if (IsLocallyControlled())
	{
		DBDGameState->SetGeneratorCustomDepth(false);
		DBDGameState->SetHangerCustomDepth(true);
	}


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
	CarriedSurvivor->GetCharacterMovement()->SetMovementMode(MOVE_None);
}

void AKiller::DropDownSurvivor()
{
	if (IsAttacking() || bStunned)
	{
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("DropDownSurvivor"));
	// 옮기고 있는 생존자가 있을 경우 해당 생존자를 놓음
	if (CarriedSurvivor)
	{
		MulticastRPC_DropDownSurvivor();
	}
}

void AKiller::ServerRPC_DropDownSurvivor_Implementation()
{
	DropDownSurvivor();
}

void AKiller::MulticastRPC_DropDownSurvivor_Implementation()
{
	// post process
	if (IsLocallyControlled())
	{
		DBDGameState->SetGeneratorCustomDepth(true);
		DBDGameState->SetHangerCustomDepth(false);
	}

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

void AKiller::HangSurvivorOnHook()
{
	// 공격 중이나 스턴 상태일 때는 실행하지 않음
	if (IsAttacking() || bStunned)
	{
		return;
	}

	if (NearGimmick->GetGimmickName() != TEXT("Hanger"))
		return;

	if (CarriedSurvivor)
	{
		MulticastRPC_HangSurvivorOnHook();
	}
}

void AKiller::ServerRPC_HangSurvivorOnHook_Implementation()
{
	HangSurvivorOnHook();
	DisableInput(Cast<APlayerController>(GetController()));
}

void AKiller::MulticastRPC_HangSurvivorOnHook_Implementation()
{
	// post process
	if (IsLocallyControlled())
	{
		DBDGameState->SetHangerCustomDepth(false);
		DBDGameState->SetGeneratorCustomDepth(true);
	}

	OnHangSurvivor.ExecuteIfBound(NearGimmick);
	PlayAnimMontage(KillerMontage, 1.0f, FName("HangSurvivorOnHook"));
}
