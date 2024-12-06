// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "DBDCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ADBDCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;


	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** change possess survivor vs killer */
	// todo: proto type 에서만 사용할 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeCharacterAction;

protected:
	/** Parkour Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParkourInputAction;

	// 파쿠르 몽타주
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ParkourMontage;

	UPROPERTY(VisibleAnywhere, Category = "SurvivorState")
	int32 SurvivorHp;


	// 파쿠르 제어 변수 (각 생존자와 살인마 클래스 내에서 사용)
	bool bIsFindWindows = false; // 창문을 찾았니? -> 창문을 찾았으면 : 베지에 곡선 좌표 설정 
	bool bIsInteractWindows = false; // 창문과 상호작용 했니? -> 했으면 : 파쿠르 애니메이션 실행
	bool bIsFindPallet = false; // 창문을 찾았니? -> 창문을 찾았으면 : 베지에 곡선 좌표 설정 
	bool bIsInteractPallet = false; // 창문과 상호작용 했니? -> 했으면 : 파쿠르 애니메이션 실행
	bool bIsPushKey = false; // 키를 눌렀니? -> 했으면 : 파쿠르 애니메이션 실행
	bool bIsParkour = false; // 파쿠르 가능하니?

	// 베지에 곡선 좌표 변수 P0 : 시작점, P1 : 제어점1, P2 : 제어점2, P3 : 끝점
	FVector vP0;
	FVector vP1;
	FVector vP2;
	FVector vP3;

	UPROPERTY()
	float ParkourSpeed = 1.0f;

	void ParkourFunc();
	UFUNCTION(Server, Reliable)
	void Server_ParkourFunc();
	// 서버가 베지에 곡선 좌표를 모든 클라이언트에게 전달하는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ParkourFunc();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Parkour2Func();


	// 베지에 곡선을 활용한 파쿠르 애니메이션 만들기

	void SetBezierPoint();
	// 서버에게 베지에 곡선 좌표 설정 요청하는 함수
	UFUNCTION(Server, Reliable)
	void Server_ReportBezierPoints();
	// 서버가 베지에 곡선 좌표를 모든 클라이언트에게 전달하는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BroadcastBezierPoints();

public:
	// 파쿠르 애니메이션 종료 함수
	void FinishParkourFunc();

	UFUNCTION(Server, Reliable)
	void Server_FinishParkourFunc();
	// 서버가 베지에 곡선 좌표를 모든 클라이언트에게 전달하는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FinishParkourFunc();


protected:
	FVector FCalculateBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2, const FVector& p3);
	void FMoveAlongQuadraticBezier(float DeltaTime);

public:
	ADBDCharacter();

	



	UFUNCTION()
	void OnParkourMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(Replicated)
	TScriptInterface<class IDBD_Interface_Gimmick> NearGimmick;

protected:
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	// todo: proto type 에서만 사용할 함수
	/** Called for changing character input */
	// void ChangeCharacter();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	virtual bool IsAttacking() const { return false; }

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
