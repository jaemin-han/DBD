// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UGameOverUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_GameOver;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WS_Interaction;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;

protected:
	virtual void NativeConstruct() override;

public:
	void InitForKiller();
	void InitForSurvivor();

private:
	// Btn_Restart Clicked
	UFUNCTION()
	void OnBtnRestartClicked();

	// Btn_Quit Clicked
	UFUNCTION()
	void OnBtnQuitClicked();

private:
	UPROPERTY()
	class UDBDGameInstance* GameInstance;
};


