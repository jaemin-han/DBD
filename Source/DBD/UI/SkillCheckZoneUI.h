// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillCheckZoneUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API USkillCheckZoneUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdatePercent(float percent);
	void SetPercent(float percent);

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_SkillCheckZone;

	// Material Instance 가져오기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialInstanceDynamic* MI_SkillCheckZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	float Percent = 0;
};
