// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UButton* GamePlay_Btn;

	virtual void NativeConstruct() override;

	// game play button clicked
	UFUNCTION()
	void OnGamePlayBtnClicked();
	

	// level name
	FName LevelName{"ProtoLevel"};
};
