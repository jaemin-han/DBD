// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyPlayerState.h"

ALobbyPlayerState::ALobbyPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100;
}
