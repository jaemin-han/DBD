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
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


	/** Parkour Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParkourInputAction;

	// 파쿠르 몽타주
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ParkourMontage;

	// 파쿠르 제어 변수 (각 생존자와 살인마 클래스 내에서 사용)
	bool bIsSearchWindows = false;			// 창문을 찾았니? -> 창문을 찾았으면 : 베지에 곡선 좌표 설정 
	bool bIsInteractWindows = false;		// 창문과 상호작용 했니? -> 했으면 : 파쿠르 애니메이션 실행
	bool bIsPushKey = false;				// 키를 눌렀니? -> 했으면 : 파쿠르 애니메이션 실행
	bool bIsParkour = false;				// 파쿠르 가능하니?

	// 베지에 곡선 좌표 변수 P0 : 시작점, P1 : 제어점1, P2 : 제어점2, P3 : 끝점
	UPROPERTY(Replicated)
	FVector vP0;
	UPROPERTY(Replicated)
	FVector vP1;
	UPROPERTY(Replicated)
	FVector vP2;
	UPROPERTY(Replicated)
	FVector vP3;



public:
	ADBDCharacter();


protected:
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
/** Input Function*/
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void ParkourFunc();
	UFUNCTION(Server, Reliable)
	void ServerRPC_ParkourFunc();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ParkourFunc();
	UFUNCTION(Client, Reliable)
	void ClientRPC_ParkourFunc();
public:
	void FinishParkourFunc();
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPC_FinishParkourFunc();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FinishParkourFunc();

	// todo: proto type 에서만 사용할 함수s
	/** Called for changing character input */
	void ChangeCharacter();
/** Input Function*/





protected:
	// 베지에 곡선을 활용한 파쿠르 애니메이션 만들기
	void SetBezierPoint(class IDBD_Interface_Gimmick* gimmick);
	//UFUNCTION(Client, Reliable)
	//void ClientRPC_SetBezierPoint(class IDBD_Interface_Gimmick* gimmick);

	FVector FCalculateBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2, const FVector& p3);
	void FMoveAlongQuadraticBezier(float DeltaTime);
	UFUNCTION(Server, Reliable)
	void ServerRPC_FMoveAlongQuadraticBezier();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FMoveAlongQuadraticBezier(float DeltaTime);

	UFUNCTION(Client, Reliable)
	void ClientRPC_FMoveAlongQuadraticBezier(float DeltaTime);
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
