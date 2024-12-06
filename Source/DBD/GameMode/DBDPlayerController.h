// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DBDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ADBDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeToSpector();
};
