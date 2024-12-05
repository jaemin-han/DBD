// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBD_Interface_Gimmick.h"
#include "GameFramework/Actor.h"
#include "EscapeZone.generated.h"

UCLASS()
class DBD_API AEscapeZone : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	class UBoxComponent* BoxComp;

public:
	// Sets default values for this actor's properties
	AEscapeZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Interaction(APawn* Caller = nullptr) override;
	virtual void FailedInteraction() override;
	virtual FString GetGimmickName() override { return Name; };
	virtual FString GetInteractKey() override { return InteractKey; };

private:
	FString Name = TEXT("EscapeZone");
	FString InteractKey = TEXT("None");

public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                             AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                             bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
	class ADBDGameState* GameState;
};
