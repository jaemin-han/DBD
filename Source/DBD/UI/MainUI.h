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
	class UWidgetSwitcher* WidgetSwitcher;

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	class UDBDGameInstance* GameInstance;
	// level name
	FName LevelName{"SurvivorTestLevel"};
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionBlock> SessionBlockClass;

#pragma region MainMenu

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Killer;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Survivor;

	UFUNCTION()
	void OnKillerClicked();
	UFUNCTION()
	void OnSurvivorClicked();

#pragma endregion

#pragma region KillerMenu

private:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ETB_SessionName;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_CreateLobby;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Back;

private:
	UFUNCTION()
	void OnCreateLobbyClicked();
	UFUNCTION()
	void OnBackClicked();

#pragma endregion

#pragma region SurvivorMenu

private:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* SB_SessionList;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Search;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Back2;

private:
	UFUNCTION()
	void OnSearchLobbyClicked();
	UFUNCTION()
	void OnAddSession(int32 Index, FString Info);
	
#pragma endregion
};
