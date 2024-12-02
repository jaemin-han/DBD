// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionBlock.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionBlock::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind the buttons
	Btn_Join->OnClicked.AddDynamic(this, &USessionBlock::OnJoinClicked);
}

void USessionBlock::OnJoinClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Clicked"));
	OnButtonClicked.ExecuteIfBound(SessionIndex);
}

void USessionBlock::SetInfo(int32 Index, FString Info)
{
	SessionIndex = Index;
	Txt_SessionBlock->SetText(FText::FromString(Info));
}
