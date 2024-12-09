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


public:
	UFUNCTION()
	void OnClickedBtnReady();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedBtnReady();


	UFUNCTION()
	void OnClickedImagePlayer();

	UFUNCTION()
	void OnCLickedImagePlayerColor();

public:
	void AddSurvivorCountUI(APlayerState* ps);
	void AddKillerCountUI(APlayerState* ps);

	void SetMyPlayerState();

	UFUNCTION()
	void SetPrimaryColor_R(float Value);
	UFUNCTION()
	void SetPrimaryColor_G(float Value);
	UFUNCTION()
	void SetPrimaryColor_B(float Value);
	UFUNCTION()
	void SetSecondaryColor_R(float Value);
	UFUNCTION()
	void SetSecondaryColor_G(float Value);
	UFUNCTION()
	void SetSecondaryColor_B(float Value);

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



	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Player_Color;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Canvas_PlayerColor;
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_PColor_R;
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_PColor_G;
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_PColor_B;

	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_SColor_R;
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_SColor_G;
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_SColor_B;

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
	bool bIsClickedImagePlayerColor = false;

	class ALobbyPlayerState* LobbyPlayerState;
};