// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionBlock.generated.h"

DECLARE_DELEGATE_OneParam(FOnButtonClicked, int32);

/**
 * 
 */
UCLASS()
class DBD_API USessionBlock : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Join;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_SessionBlock;

public:
	UFUNCTION()
	void OnJoinClicked();

	void SetInfo(int32 Index, FString Info);

private:
	int32 SessionIndex;

public:
	FOnButtonClicked OnButtonClicked;
};
