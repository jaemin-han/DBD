// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBD_Interface_Gimmick.h"
#include "GameFramework/Actor.h"
#include "Hanger.generated.h"

UCLASS()
class DBD_API AHanger : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHanger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction(AActor* Caller = nullptr) override;
};
