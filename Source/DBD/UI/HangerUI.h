// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HangerUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UHangerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SacrifaceGage;

public:
	// SacrifaceGage 의 percent 를 설정하는 함수
	void SetSacrifaceGagePercent(const float Percent) const;
};
