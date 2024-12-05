// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

/**
 * LobbyLevel 에서만 존재하는 GameState
 * 
 * 플레이어들의 위치를 저장해주고 들어올 경우 해당 위치에 고정시킨다.
 * 플레이어 접속 현황?을 보여준다.
 * 각 플레이어들의 Ready 상태를 관리한다.
 * 
 */
UCLASS()
class DBD_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	


public:
	// 플레이어들의 위치값을 저장해줄 배열
	UPROPERTY(EditAnywhere)
	TArray<FVector> PlayerLocations;	// [0] = Killer, [1] = Survivor1, [2] = Survivor2, [3] = Survivor3, [4] = Survivor4

	// 플레이어들의 Ready 상태를 저장해줄 배열
	UPROPERTY(EditAnywhere)
	TArray<bool> PlayerReady;	// [0] = Survivor1, [1] = Survivor2, [2] = Survivor3, [3] = Survivor4, [4] = Killer
};
