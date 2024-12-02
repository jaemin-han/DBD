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


	inline bool GetIsFullGauge() const {return IsFullGauge;}

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExitGauge;

	UPROPERTY(Replicated)
	float Percent = 0;

	UPROPERTY(Replicated)
	bool IsFullGauge = false;
};
