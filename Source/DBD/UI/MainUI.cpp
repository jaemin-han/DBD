// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "SessionBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameMode/DBDGameInstance.h"

class ADBDGameMode;

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = Cast<UDBDGameInstance>(GetGameInstance());
	// bind delegate for adding session
	GameInstance->OnAddSession.BindUObject(this, &UMainUI::OnAddSession);

	// Bind the buttons

	// MainMenu
	Btn_Killer->OnClicked.AddDynamic(this, &UMainUI::OnKillerClicked);
	Btn_Survivor->OnClicked.AddDynamic(this, &UMainUI::OnSurvivorClicked);

	// KillerMenu
	Btn_CreateLobby->OnClicked.AddDynamic(this, &UMainUI::OnCreateLobbyClicked);
	Btn_Back->OnClicked.AddDynamic(this, &UMainUI::OnBackClicked);

	// SurvivorMenu
	Btn_Search->OnClicked.AddDynamic(this, &UMainUI::OnSearchLobbyClicked);
	Btn_Back2->OnClicked.AddDynamic(this, &UMainUI::OnBackClicked);
}

void UMainUI::OnKillerClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UMainUI::OnSurvivorClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
}

void UMainUI::OnCreateLobbyClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Create Lobby Clicked"));
	GameInstance->CreateSession(ETB_SessionName->GetText().ToString(), 5);
}

void UMainUI::OnBackClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void UMainUI::OnSearchLobbyClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Search Lobby Clicked"));
	SB_SessionList->ClearChildren();

	GameInstance->FindOtherSessions();
}

void UMainUI::OnAddSession(int32 Index, FString Info)
{
	USessionBlock* SessionBlock = CreateWidget<USessionBlock>(this, SessionBlockClass);
	SessionBlock->SetInfo(Index, Info);
	SessionBlock->OnButtonClicked.BindUObject(GameInstance, &UDBDGameInstance::JoinOtherSession);
	SB_SessionList->AddChild(SessionBlock);
}


