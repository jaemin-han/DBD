// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "DBD_Player.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ADBD_Player : public ADBDCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Run();
	void RunStop();
	void Crouch();
	void CrouchStop();
	void PushInteractGenerator();
	void NonPushInteractGenerator();
	void Parkour();
	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 임시용
	void PlusHp();
	void MinusHp();


	void Interaction();

public:
	// 생존자의 HP 업데이트 함수
	void UpdateHP(int32 Damage);
	void UpdateSpeed();
	void ParkourFinish();


	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline int32 GetHealth() const {return Health;}
	inline bool GetIsRunning() const {return IsRunning;}
	inline bool GetCrouching() const {return IsCrouching;}
	inline bool GetIsInteractGenerator() const {return IsInteractGenerator;}

private:
	// 임시용 체력 변경 키값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PlusHpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MinusHpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParkourAction;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* ParkourMontage;



	// 생존자 체력
	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 Health; // 3 : 멀쩡 2 : 부상 1 : 중상 0 : 죽음

	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 MaxHealth = 3; 

	// 뛰는지 안뛰는지 체크 변수
	bool IsRunning = false;
	// Crouch 상태인지 체크 변수
	bool IsCrouching = false;
	// Generator 상호작용 체크 변수 
	bool IsInteractGenerator = false;
	bool IsInteractWindows = false;


	// LineTrace와 Actor들과 닿았는지 체크하는 변수
	bool IsReachGenerator = false;
	bool IsReachWindows = false;
};