// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	ALobbyPlayerState(const FObjectInitializer& ObjectInitializer);


public:
	FString GetPlayerUniqueName();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GetPlayerUniqueName();
public:
	UPROPERTY()
	FString PlayerUniqueName;

};
