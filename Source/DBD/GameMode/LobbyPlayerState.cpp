// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyPlayerState.h"

ALobbyPlayerState::ALobbyPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100;
}

FString ALobbyPlayerState::GetPlayerUniqueName()
{
	
	if (HasAuthority())
	{
		PlayerUniqueName = FString::Printf(TEXT("Killer %d"), GetPlayerId());
	}
	else
	{
		Multi_GetPlayerUniqueName();
	}
	return PlayerUniqueName;
}

void ALobbyPlayerState::Multi_GetPlayerUniqueName_Implementation()
{
	PlayerUniqueName = FString::Printf(TEXT("Survivor %d"), GetPlayerId());
}
