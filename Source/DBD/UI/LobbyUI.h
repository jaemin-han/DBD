// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
	
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickedBtnReady();

	UFUNCTION()
	void OnClickedImagePlayer();


public:
	void AddSurvivorCountUI(APlayerState* ps);
	void AddKillerCountUI(APlayerState* ps);

	void SetMyPlayerState();

public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Player;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Canvas_PlayerCount;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VBox_SurvivorCount;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VBox_KillerCount;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Ready;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Ready;

	UPROPERTY()
	class ALobbyPlayerState* myPlayerState;
	FTimerHandle playerStateHandle;




	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPlayerCountUI> playerCountUIFactory;

	UPROPERTY()
	TArray<class UPlayerCountUI*> allSurvivorState;
	UPROPERTY()
	TArray<class UPlayerCountUI*> allKillerState;

	bool bIsClickedImagePlayer = false;
};