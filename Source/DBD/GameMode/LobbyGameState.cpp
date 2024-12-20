﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameState.h"

#include "LobbyPlayerState.h"
#include "UI/LobbyUI.h"
#include "UI/PlayerCountUI.h"
#include "Components/Button.h"
#include "Components/Image.h"

void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (LobbyUI)
	{
		LobbyUI->RemoveFromParent();
	}
}

ULobbyUI* ALobbyGameState::GetLobbyUI()
{
	if (LobbyUI == nullptr)
	{
		// GameUI 생성
		LobbyUI = CreateWidget<ULobbyUI>(GetWorld(), LobbyUIFactor);
		LobbyUI->AddToViewport();
	}

	return LobbyUI;
}