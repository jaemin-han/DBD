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
	
	virtual void NativeConstruct() override;

public:
	void UpdateGauge(float time);
	bool UpdateRoundPercent(float time);

	void VisibleRondGauge(bool IsVisible);


	inline bool GetIsFullGauge() const {return IsFullGauge;}
	inline void SetActivatedRoundGauge(bool value) { ActivatedRoundGauge = value; }
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* Gauge;

	UPROPERTY(meta = (BindWidget))
	class URoundGaugeUI* RoundGauge;





private:
	float Percent = 0;
	bool IsFullGauge = false;
	bool ActivatedRoundGauge = false;
};
