// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Decal.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ADecal : public ADecalActor
{
	GENERATED_BODY()
	
	ADecal();


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void DestroyDecal();

	class UMaterialInstanceDynamic* MI_Decal;

	float DestroyTimer = 0.0f;
	float Percent = 1.0f;
};
