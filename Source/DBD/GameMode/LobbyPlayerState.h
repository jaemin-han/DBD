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

	virtual void BeginPlay() override;
	
	ALobbyPlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FString GetPlayerUniqueName();

	void SetPlayerUniqueName();

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerUniqueName();


	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlayerUniqueName(const FString& name);



	void OnClickedBtnReady();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedBtnReady();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_OnClickedBtnReady(class ALobbyGameState* lgs);


	void SetMeshColor();


public:
	UPROPERTY(Replicated)
	FString PlayerUniqueName;

	UPROPERTY()
	class ALobbyGameState* LobbyGameState;

	bool bIsSurvivorReady = false;
	bool bIsAllReady = false;
};
