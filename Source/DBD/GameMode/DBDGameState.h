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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void AddDynamicMaterialToPostProcessVolume();

public:
	UPROPERTY(VisibleAnywhere, Category = "Actors")
	TArray<class AGenerator*> Generators;
	void SetGeneratorCustomDepth(bool bIsVisible);

	UPROPERTY(VisibleAnywhere, Category = "Actors")
	TArray<class AHanger*> Hangers;
	void SetHangerCustomDepth(bool bIsVisible);

public:
	void InitArrays();
	// SurvivorCount getter, setter
	int32 GetSurvivorCount() const { return SurvivorCount; }
	void SetSurvivorCount(int32 Value) { SurvivorCount = Value; }
	// game over
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_GameOver();

private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Survivor")
	int32 SurvivorCount = 0;
	
};
