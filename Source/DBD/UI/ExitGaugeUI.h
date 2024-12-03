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
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void UpdateExitGauge(float time);

	UFUNCTION(Server, Reliable)
	void Server_UpdateExitGauge();

	UFUNCTION(Client, Reliable)
	void Client_UpdateExitGauge(float time);


	void SetGaugePerent(float per) {Percent += per;}
	void SetIsFullGauge(bool isFull) {IsFullGauge = isFull;}


	class UProgressBar* GetExitGauge() const {return ExitGauge;}
	inline bool GetIsFullGauge() const {return IsFullGauge;}
	float GetGaugePercent() const	{return Percent;}


private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExitGauge;

	float Percent = 0;

	bool IsFullGauge = false;

	
};
