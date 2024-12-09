// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverUI.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "GameMode/DBDGameInstance.h"

void UGameOverUI::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = Cast<UDBDGameInstance>(GetGameInstance());

	// bind events
	Btn_Restart->OnClicked.AddDynamic(this, &UGameOverUI::OnBtnRestartClicked);
	Btn_Quit->OnClicked.AddDynamic(this, &UGameOverUI::OnBtnQuitClicked);
}

void UGameOverUI::InitForKiller()
{
	// widget swicher index 0 is for killer
	WS_Interaction->SetActiveWidgetIndex(0);
}

void UGameOverUI::InitForSurvivor()
{
	// widget swicher index 1 is for survivor
	WS_Interaction->SetActiveWidgetIndex(1);
}

void UGameOverUI::OnBtnRestartClicked()
{
}

void UGameOverUI::OnBtnQuitClicked()
{
}
