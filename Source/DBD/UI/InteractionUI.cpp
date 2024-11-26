// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InteractionUI.h"

#include "Components/TextBlock.h"

void UInteractionUI::SetGimmickName(FString string)
{
	FText str = FText::FromString(string);
	GimmickName->SetText(str);
}

void UInteractionUI::SetInteractKey(FString string)
{
	FText str = FText::FromString(string);
	InteractKey->SetText(str);
}
