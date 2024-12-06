// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API APlayGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "PlayerClass")
	TSubclassOf<class ADBDCharacter> KillerClass;
	UPROPERTY(EditAnywhere, Category = "PlayerClass")
	TSubclassOf<class ADBD_Player> SurvivorClass;

protected:
	virtual void BeginPlay() override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};
