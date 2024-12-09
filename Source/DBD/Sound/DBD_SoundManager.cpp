// Fill out your copyright notice in the Description page of Project Settings.


#include "Sound/DBD_SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "AudioDevice.h"

UDBD_SoundManager* UDBD_SoundManager::SoundInstance = nullptr;

UDBD_SoundManager* UDBD_SoundManager::Get()
{
	if (not SoundInstance)
	{
		SoundInstance = NewObject<UDBD_SoundManager>();
	}
	return SoundInstance;
}

void UDBD_SoundManager::Initialize(UWorld* world)
{
	World = world;

	// 예시: 사운드 큐를 미리 로드
	//Sounds.Add("Sound_Explosion", LoadObject<USoundBase>(nullptr, TEXT("/Game/DBD/Sound/ExplosionSound")));
	//Sounds.Add("Sound_Generator", LoadObject<USoundBase>(nullptr, TEXT("/Game/DBD/Sound/generatorSound")));
	//Sounds.Add("Sound_HeartBeat", LoadObject<USoundBase>(nullptr, TEXT("/Game/DBD/Sound/HeartBeat")));
}

UAudioComponent* UDBD_SoundManager::PlaySound(FName name, FVector location, float volume, float pitch, bool bLoop)
{
	if (not World) return nullptr;

	if (not Sounds.Contains(name)) return nullptr;
	USoundBase* sound = Sounds[name];



	return UGameplayStatics::SpawnSoundAtLocation(World, sound, location, FRotator::ZeroRotator, volume, pitch);
}

void UDBD_SoundManager::StopSound(UAudioComponent* sound)
{
	if (sound and sound->IsPlaying())
	{
		sound->Stop();
	}
}

void UDBD_SoundManager::SetVolume(float volume)
{
}

void UDBD_SoundManager::SetMasterVolume(float volume)
{
	//GEngine->GetMainAudioDevice()->SetTransientMasterVolume(volume);
}
