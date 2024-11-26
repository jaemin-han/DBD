// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UInteractionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetGimmickName(FString string);
	void SetInteractKey(FString string);

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GimmickName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractKey;
};
