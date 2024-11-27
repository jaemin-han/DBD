// Copyright Epic Games, Inc. All Rights Reserved.

#include "DBDCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameMode/DBDGameMode.h"

#include "Gimmick/DBD_Interface_Gimmick.h"
#include "Animation/AnimInstance.h"
#include "Gimmick/Windows.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADBDCharacter

ADBDCharacter::ADBDCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ADBDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ADBDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsParkour)
	{
		FMoveAlongQuadraticBezier(DeltaTime);
	}
}

void ADBDCharacter::SetBezierPoint(class IDBD_Interface_Gimmick* gimmick)
{
	if (AWindows* window = Cast<AWindows>(gimmick))
	{
		float dist = FVector::Distance(GetActorLocation(), window->GetActorLocation());
		// 창문의 위치를 가져와서 베지에 곡선 좌표 설정
		vP0 = GetActorLocation();
		vP1 = vP0 + GetActorUpVector() * 120.0f;
		vP2 = vP1 + GetActorForwardVector() * dist * 2;
		vP3 = vP0 + GetActorForwardVector() * dist * 2;
	}
}

FVector ADBDCharacter::FCalculateBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2,
                                             const FVector& p3)
{
	float oneMinusT = 1.0f - t;
	FVector result = oneMinusT * oneMinusT * oneMinusT * p0 +
		3 * oneMinusT * oneMinusT * t * p1 +
		3 * oneMinusT * t * t * p2 +
		t * t * t * p3;

	return result;
}

void ADBDCharacter::FMoveAlongQuadraticBezier(float DeltaTime)
{
	// 한번 선언하면 다시 초기화 되지 않는 변수 선언
	static float time = 0.0f;
	time += DeltaTime * ParkourSpeed;


	// 2차 베지에 곡선 계산
	// float t = FMath::Clamp(time / 0.5f, 0.0f, 1.0f);
	// FVector newPos = CalculateQuadraticBezierPoint(t, P0, P1, P2);

	// 3차 베지에 곡선 계산
	float t = FMath::Clamp(time, 0.0f, 1.0f);
	FVector newPos = FCalculateBezierPoint(t, vP0, vP1, vP2, vP3);

	SetActorLocation(newPos);
	if (t >= 1.0f)
	{
		time = 0.0f;
		bIsParkour = false;
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADBDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADBDCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADBDCharacter::Look);

		// todo: proto type 에서만 사용할 함수
		// // Change Character
		// EnhancedInputComponent->BindAction(ChangeCharacterAction, ETriggerEvent::Started, this,
		//                                    &ADBDCharacter::ChangeCharacter);

		EnhancedInputComponent->BindAction(ParkourInputAction, ETriggerEvent::Started, this,
		                                   &ADBDCharacter::ParkourFunc);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void ADBDCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADBDCharacter, NearGimmick);
}

void ADBDCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADBDCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADBDCharacter::ParkourFunc()
{
	if (not bIsSearchWindows) return;
	if (bIsPushKey) return;

	if (IsAttacking())
		return;

	SetBezierPoint(NearGimmick.GetInterface());

	PlayAnimMontage(ParkourMontage, ParkourSpeed, TEXT("Parkour"));
	bIsSearchWindows = false;
	bIsParkour = true;
	GetCharacterMovement()->DisableMovement();
	bIsPushKey = true;
}

void ADBDCharacter::FinishParkourFunc()
{
	StopAnimMontage(ParkourMontage);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bIsPushKey = false;
}

void ADBDCharacter::OnParkourMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ParkourMontage)
	{
		FinishParkourFunc();
	}
}

// void ADBDCharacter::ChangeCharacter()
// {
// 	// 본인이 Survivor일 경우 Killer로, Killer일 경우 Survivor로 변경
// 	if (ADBDGameMode* GameMode = Cast<ADBDGameMode>(GetWorld()->GetAuthGameMode()))
// 	{
// 		GameMode->ChangeCharacter();
// 	}
// }
