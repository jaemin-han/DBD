// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DBDGameMode.generated.h"

UCLASS(minimalapi)
class ADBDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADBDGameMode();

	// main widget class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> MainUIClass;

	// todo: proto type 에서만 사용할 함수
	UFUNCTION()
	void ChangeCharacter();
};
