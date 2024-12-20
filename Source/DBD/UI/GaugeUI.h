﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GaugeUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UGaugeUI : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* Gauge;

	UPROPERTY(meta = (BindWidget))
	class URoundGaugeUI* RoundGauge;

	UPROPERTY(meta = (BindWidget))
	class USkillCheckZoneUI* SkillCheckZone;
	
public:
	void SetPercent(float value);
};
