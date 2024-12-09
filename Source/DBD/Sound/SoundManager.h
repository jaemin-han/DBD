// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

UCLASS()
class DBD_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Sound")
	void AddSound(FName name, class USoundBase* sound);
	UFUNCTION(Category = "Sound")
	void PlaySound(FName name);
	UFUNCTION(Category = "Sound")
	void StopSound(FName name);
	UFUNCTION(Category = "Sound")
	void SetSoundVolume(FName name, float volume);



public:
	UPROPERTY(EditAnywhere, Category = "Sound")
	TMap<FName, UAudioComponent*> Sounds;
};
