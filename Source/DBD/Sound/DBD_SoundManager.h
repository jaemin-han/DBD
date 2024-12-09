// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DBD_SoundManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DBD_API UDBD_SoundManager : public UObject
{
	GENERATED_BODY()
	
public:
	// Singleton 접근 메서드
	static UDBD_SoundManager* Get();

	// SoundManager 초기화
	void Initialize(class UWorld* world);

	// 사운드 재생
	class UAudioComponent* PlaySound(FName name, FVector location, float volume = 1.0f, float pitch = 1.0f, bool bLoop = false);

	// 사운드 정리
	void StopSound(class UAudioComponent* sound);

	// 볼륨 설정
	void SetVolume(float volume);
	// 전체 볼륨 설정
	void SetMasterVolume(float volume);

private:
	// 사운드 맵
	UPROPERTY(EditAnywhere)
	TMap<FName, class USoundBase*> Sounds;
	//TMap<FName, class UAudioComponent*> Sounds;

	// 월드참조
	class UWorld* World;

	// Singleton 인스턴스
	static UDBD_SoundManager* SoundInstance;
};
