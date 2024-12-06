// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerCountUI.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "GameMode/LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"

void UPlayerCountUI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerCountUI, Image_Check);
}

void UPlayerCountUI::Init(ALobbyPlayerState* lps)
{
	if (lps)
	{
		LobbyPlayerState = lps;
		//UE_LOG(LogTemp, Error, TEXT("[UPlayerCountUI] lpsPlayerName : %s"), *lps->GetPlayerName());
		lps->Server_SetPlayerUniqueName();
		UpdateName(lps->PlayerUniqueName);
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