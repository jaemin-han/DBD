// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExitGaugeUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UExitGaugeUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateExitGauge(float time);


	inline bool GetIsFullGauge() const {return IsFullGauge;}

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExitGauge;

	float Percent = 0;

	bool IsFullGauge = false;
};
