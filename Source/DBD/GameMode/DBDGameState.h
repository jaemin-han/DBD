// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DBDGameState.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ADBDGameState : public AGameStateBase
{
	GENERATED_BODY()

	ADBDGameState();

private:
	UPROPERTY(VisibleAnywhere)
	class APostProcessVolume* PostProcessVolume;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInstance* MaterialInstance;

protected:
	virtual void BeginPlay() override;

private:
	void AddDynamicMaterialToPostProcessVolume();
};
