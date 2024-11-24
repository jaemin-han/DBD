// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "Character/Killer.h"
#include "Components/Button.h"
#include "GameMode/DBDGameMode.h"
#include "Kismet/GameplayStatics.h"

class ADBDGameMode;

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind the buttons
	GamePlay_Btn->OnClicked.AddDynamic(this, &UMainUI::OnGamePlayBtnClicked);
}

void UMainUI::OnGamePlayBtnClicked()
{
	// open level
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}


