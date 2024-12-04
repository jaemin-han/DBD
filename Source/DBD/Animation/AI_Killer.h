﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AI_Killer.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UAI_Killer : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeInitializeAnimation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor")
	class AKiller* Killer;
	UPROPERTY(VisibleAnywhere, Category = "Gimmick")
	class AHanger* Hanger;

public:
	UFUNCTION()
	void AnimNotify_HangOnHook();
	
};
