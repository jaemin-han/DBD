// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerCountUI.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "GameMode/LobbyPlayerState.h"

void UPlayerCountUI::Init(ALobbyPlayerState* lps)
{
	if (lps)
	{
		LobbyPlayerState = lps;

		UpdateName(lps->GetPlayerName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyPlayerState is nullptr"));
	}
}

void UPlayerCountUI::UpdateName(FString name)
{
	Text_PlayerName->SetText(FText::FromString(name));
}