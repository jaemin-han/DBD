// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DBDGameInstance.generated.h"

DECLARE_DELEGATE_TwoParams(FOnAddSessionDelegate, int32, FString);
/**
 * 
 */

UCLASS()
class DBD_API UDBDGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;

public:
	// 세션 생성
	UFUNCTION(BlueprintCallable)
	void CreateSession(FString DisplayName, int32 PlayerCount);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	//  세션 파괴
	UFUNCTION(BlueprintCallable)
	void DestroySession(FString SessionName);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// 세션 검색
	UFUNCTION(BlueprintCallable)
	void FindOtherSessions();
	void OnFindSessionsComplete(bool bWasSuccessful);

	// 세션 참여
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 Index);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

public:
	// 세션의 모든 처리를 진행
	IOnlineSessionPtr SessionInterface;

	// 세션 검색 처리
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// MainUI 에서 세션을 추가할 때 사용
	FOnAddSessionDelegate OnAddSession;

	FString SName;

#pragma region GameOverUI

private:
	// 이 컴퓨터에서 플레이한 캐릭터가 킬러인지 생존자인지
	bool bIsKiller;

	// killer
	// 킬러가 살해한 생존자 몇 명?
	int32 killerKillCount;

	// Survivor
	// 생존자의 탈출 여부
	bool bIsEscaped;

public:
	bool GetIsKiller() const { return bIsKiller; };
	void SetIsKiller(const bool IsKiller) { bIsKiller = IsKiller; };

	int32 GetKillerKillCount() const { return killerKillCount; };
	void SetKillerKillCount(const int32 KillCount) { killerKillCount = KillCount; };

	bool GetIsEscaped() const { return bIsEscaped; };
	void SetIsEscaped(const bool IsEscaped) { bIsEscaped = IsEscaped; };

#pragma endregion
};
