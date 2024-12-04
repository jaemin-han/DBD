// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundGaugeUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API URoundGaugeUI : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	void UpdatePercent(float per);

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_RoundGauge;

	// Material Instance 가져오기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInstanceDynamic* MI_RoundProcessBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	float Percent = 0;
};
