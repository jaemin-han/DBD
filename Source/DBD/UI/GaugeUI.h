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

	inline void SetSkillCheckZone(float value);

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* Gauge;

	UPROPERTY(meta = (BindWidget))
	class URoundGaugeUI* RoundGauge;

	UPROPERTY(meta = (BindWidget))
	class USkillCheckZoneUI* SkillCheckZone;



private:
	class ADBD_Player* Player;

	float Start = 0;
	float Random = 0;

	float Percent = 0;
	bool IsFullGauge = false;
	bool ActivatedRoundGauge = false;

	float SkillCheckZoneStart = 0;
	bool ActivatedSkillCheckZone = false;

	bool IsSuccessSkillCheck = false;		// 스킬체크구역에서 스페이스바를 제대로 눌렀니?
};
