// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DBD_Surviori_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UDBD_Surviori_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ADBD_Player* Player;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Health;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsRunning;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsCrouching;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsGenerator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsPiggyBack;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsHang;

public:
	// Reload 애니메이션 끝나는 부분에 실행되는 함수
	UFUNCTION()
	void AnimNotify_OnParkourFinish();
	// Rescue 애니메이션 끝나는 부분에 실행되는 함수
	UFUNCTION()
	void AnimNotify_OnRescueFinish();
};