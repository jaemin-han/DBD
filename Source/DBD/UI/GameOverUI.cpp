// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GameMode/DBDGameInstance.h"

void UGameOverUI::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = Cast<UDBDGameInstance>(GetGameInstance());

	// check game instance
	if (GameInstance->GetIsKiller())
	{
		InitForKiller();
	}
	else
	{
		InitForSurvivor();
	}

	// bind events
	Btn_Restart->OnClicked.AddDynamic(this, &UGameOverUI::OnBtnRestartClicked);
	Btn_Quit->OnClicked.AddDynamic(this, &UGameOverUI::OnBtnQuitClicked);
}

void UGameOverUI::InitForKiller()
{
	// widget swicher index 0 is for killer
	WS_Interaction->SetActiveWidgetIndex(0);

	// gameinstance 에서 killer kill count 를 가져온다.
	int32 KillCount = GameInstance->GetKillerKillCount();
	// KillCount 만큼의 생존자를 희생시켰습니다. 라는 메시지를 Text_GameOver 에 출력
	Text_GameOver->SetText(FText::FromString(FString::Printf(TEXT("당신은 %d 명의 생존자를 희생시켰습니다..."), KillCount)));
}

void UGameOverUI::InitForSurvivor()
{
	// widget swicher index 1 is for survivor
	WS_Interaction->SetActiveWidgetIndex(1);

	// gameinstance 에서 isescaped 를 가져온다.
	bool IsEscaped = GameInstance->GetIsEscaped();
	// IsEscaped 가 true 이면 Text_GameOver 에 탈출에 성공했습니다. 라는 메시지를 출력
	if (IsEscaped)
	{
		Text_GameOver->SetText(FText::FromString(TEXT("탈출에 성공했습니다...")));
	}
	else
	{
		// IsEscaped 가 false 이면 Text_GameOver 에 살인마에게 잡혔습니다. 라는 메시지를 출력
		Text_GameOver->SetText(FText::FromString(TEXT("살인마에게 잡혔습니다...")));
	}
}

void UGameOverUI::OnBtnRestartClicked()
{
	GetWorld()->ServerTravel(TEXT("/Game/DBD/Level/LobbyLevel?listen"));

	// 모든 버튼 비활성화
	Btn_Restart->SetIsEnabled(false);
	Btn_Quit->SetIsEnabled(false);
}

void UGameOverUI::OnBtnQuitClicked()
{
	GameInstance->DestroySession(GameInstance->SName);

	// 모든 버튼 비활성화
	Btn_Restart->SetIsEnabled(false);
	Btn_Quit->SetIsEnabled(false);
}
