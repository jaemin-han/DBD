// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "SoundComponent.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API USoundComponent : public UAudioComponent
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

public:
	UFUNCTION(Category = "Sound")
	void AddSound(FName name, class USoundBase* newSound);
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
