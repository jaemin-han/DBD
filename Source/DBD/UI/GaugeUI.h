// Fill out your copyright notice in the Description page of Project Settings.

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
	
public:
	void UpdateGauge(float time);

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* Gauge;

	float Percent = 0;
};
