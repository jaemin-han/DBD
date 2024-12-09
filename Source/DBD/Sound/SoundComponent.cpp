// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/SoundComponent.h"
#include "Sound/SoundCue.h"


void USoundComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USoundComponent::AddSound(FName name, USoundBase* newSound)
{
	if (not newSound) return;
	if (Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = NewObject<UAudioComponent>(this))
	{
		audioComponent->SetSound(newSound);
		audioComponent->RegisterComponent();
		Sounds.Add(name, audioComponent);
	}
}

void USoundComponent::PlaySound(FName name)
{
	if (not Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = Sounds[name])
	{
		audioComponent->Play();
	}
}

void USoundComponent::StopSound(FName name)
{
	if (not Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = Sounds[name])
	{
		audioComponent->Stop();
	}
}

void USoundComponent::SetSoundVolume(FName name, float volume)
{
	if (not Sounds.Contains(name)) return;

	if (UAudioComponent* audioComponent = Sounds[name])
	{
		audioComponent->SetVolumeMultiplier(volume);
	}
}
