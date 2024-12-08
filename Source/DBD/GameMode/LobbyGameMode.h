// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * LobbyLevel에서만 존재하는 GameMode
 * LobbyLevel에서 보여지는 UI를 관리한다.
 * 플레이어들의 InputMode는 UIOnly로 고정하고 PlaySession으로 넘어갈 경우 GameOnly로 변경해준다.
 */
UCLASS()
class DBD_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	virtual void Tick(float DeltaSeconds) override;
	
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player);


private:
	int32 PlayerCount;
	
	UPROPERTY(EditAnywhere, Category = "PlayerClass")
	TSubclassOf<class ADBDCharacter> KillerClass;
	UPROPERTY(EditAnywhere, Category = "PlayerClass")
	TSubclassOf<class ADBD_Player> SurvivorClass;

	int32 index;
};
