// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/SoundManager.h"
#include "Components/AudioComponent.h"


// Sets default values
ASoundManager::ASoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoundManager::AddSound(FName name, USoundBase* sound)
{
	if (not sound) return;
	if (Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = NewObject<UAudioComponent>(this))
	{
		audioComponent->SetSound(sound);
		audioComponent->RegisterComponent();
		Sounds.Add(name, audioComponent);
	}
}

void ASoundManager::PlaySound(FName name)
{
	if (not Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = Sounds[name])
	{
		audioComponent->Play();
	}
}

void ASoundManager::StopSound(FName name)
{
	if (not Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = Sounds[name])
	{
		audioComponent->Stop();
	}
}

void ASoundManager::SetSoundVolume(FName name, float volume)
{
	if (not Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = Sounds[name])
	{
		audioComponent->SetVolumeMultiplier(volume);
	}
}

