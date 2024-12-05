// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCountUI.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UPlayerCountUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(class ALobbyPlayerState* lps);
	void UpdateName(FString name);


public:

	UPROPERTY()
	class ALobbyPlayerState* LobbyPlayerState;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_PlayerName;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Check;
};
