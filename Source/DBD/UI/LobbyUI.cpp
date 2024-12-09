// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyUI.h"

#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"

#include "PlayerCountUI.h"
#include "GameMode/LobbyPlayerState.h"
#include "GameMode/LobbyGameState.h"


class FPlayerCountUISort
{
public:
	bool operator() (const UPlayerCountUI& a, const UPlayerCountUI& b) const
	{
		return (a.LobbyPlayerState->GetPlayerId() < b.LobbyPlayerState->GetPlayerId()); // 오름차순
	}
};

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Log, TEXT("NativeConstruct called"));

	LobbyPlayerState = Cast<ALobbyPlayerState>(GetWorld()->GetGameState());



	Image_Player->OnMouseButtonDownEvent.BindUFunction(this, "OnClickedImagePlayer");

	// Btn_Ready이 눌렸을때 호출되는 함수
	Btn_Ready->OnClicked.AddDynamic(this, &ULobbyUI::Server_OnClickedBtnReady);

	// Image_Player_Color이 눌렸을때 호출되는 함수
	Image_Player_Color->OnMouseButtonDownEvent.BindUFunction(this, "OnCLickedImagePlayerColor");

	//Slider_PColor_R->OnValueChanged.AddDynamic(this, &ULobbyUI::SetPrimaryColor_R);
	//Slider_PColor_G->OnValueChanged.AddDynamic(this, &ULobbyUI::SetPrimaryColor_G);
	//Slider_PColor_B->OnValueChanged.AddDynamic(this, &ULobbyUI::SetPrimaryColor_B);
	//
	//Slider_SColor_R->OnValueChanged.AddDynamic(this, &ULobbyUI::SetSecondaryColor_R);
	//Slider_SColor_G->OnValueChanged.AddDynamic(this, &ULobbyUI::SetSecondaryColor_G);
	//Slider_SColor_B->OnValueChanged.AddDynamic(this, &ULobbyUI::SetSecondaryColor_B);



	// 나의 PlayerState 찾아오는 Timer 돌리자
	GetWorld()->GetTimerManager().SetTimer(playerStateHandle, this, &ULobbyUI::SetMyPlayerState, 0.1f, true);
}

void ULobbyUI::OnClickedBtnReady()
{
	UE_LOG(LogTemp, Warning, TEXT("[LobbyUI] Btn_Ready Clicked"));

	ALobbyPlayerState* ps = Cast<ALobbyPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	ps->Server_OnClickedBtnReady();
}

void ULobbyUI::Server_OnClickedBtnReady_Implementation()
{
	OnClickedBtnReady();
}

void ULobbyUI::OnClickedImagePlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("[LobbyUI] Image_Player Clicked"));

	bIsClickedImagePlayer = !bIsClickedImagePlayer;
	Canvas_PlayerColor->SetVisibility(ESlateVisibility::Hidden);
	if (bIsClickedImagePlayer)
	{
		Canvas_PlayerCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Canvas_PlayerCount->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULobbyUI::OnCLickedImagePlayerColor()
{
	UE_LOG(LogTemp, Warning, TEXT("[LobbyUI] Image_Player_Color Clicked"));

	bIsClickedImagePlayerColor = !bIsClickedImagePlayerColor;
	Canvas_PlayerCount->SetVisibility(ESlateVisibility::Hidden);
	if (bIsClickedImagePlayerColor)
	{
		Canvas_PlayerColor->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Canvas_PlayerColor->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ULobbyUI::AddSurvivorCountUI(APlayerState* ps)
{
	if (ps == nullptr) return;

	// 만약에 ps 가 이미 추가 되어있다면 함수를 나가자.
	bool isExist = allSurvivorState.ContainsByPredicate([ps](UPlayerCountUI* p) {
		return p->LobbyPlayerState == ps;
		});
	if (isExist) return;

	VBox_SurvivorCount->ClearChildren();

	UPlayerCountUI* pcUI = CreateWidget<UPlayerCountUI>(GetWorld(), playerCountUIFactory);
	ALobbyPlayerState* lobbyPlayerState = Cast<ALobbyPlayerState>(ps);
	
	pcUI->Init(lobbyPlayerState);
	//만들어진 pcUI를 나만의 Array에 추가하기
	allSurvivorState.Add(pcUI);
	// allPlayerState를 PlayerID 기준으로 정렬
	//allPlayerState.Sort(FPlayerCountUISort());
	
	for (int32 i = 0; i < allSurvivorState.Num(); i++)
	{
		VBox_SurvivorCount->AddChild(allSurvivorState[i]);
		UVerticalBoxSlot* slot = Cast<UVerticalBoxSlot>(allSurvivorState[i]->Slot);
		slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	for (int32 i = 0; i < 6 - allSurvivorState.Num(); i++)
	{
		USizeBox* sizeBox = NewObject<USizeBox>(this);
		VBox_SurvivorCount->AddChild(sizeBox);
		UVerticalBoxSlot* slot = Cast<UVerticalBoxSlot>(sizeBox->Slot);
		slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}
}

void ULobbyUI::AddKillerCountUI(APlayerState* ps)
{
	if (ps == nullptr) return;

	// 만약에 ps 가 이미 추가 되어있다면 함수를 나가자.
	bool isExist = allKillerState.ContainsByPredicate([ps](UPlayerCountUI* p) {
		return p->LobbyPlayerState == ps;
		});
	if (isExist) return;

	UPlayerCountUI* pcUI = CreateWidget<UPlayerCountUI>(GetWorld(), playerCountUIFactory);
	ALobbyPlayerState* lobbyPlayerState = Cast<ALobbyPlayerState>(ps);
	
	pcUI->Init(lobbyPlayerState);
	//만들어진 pcUI를 나만의 Array에 추가하기
	allKillerState.Add(pcUI);
	// allPlayerState를 PlayerID 기준으로 정렬
	//allPlayerState.Sort(FPlayerCountUISort());
	
	for (int32 i = 0; i < allKillerState.Num(); i++)
	{
		VBox_KillerCount->AddChild(allKillerState[i]);
		UVerticalBoxSlot* slot = Cast<UVerticalBoxSlot>(allKillerState[i]->Slot);
		slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	USizeBox* sizeBox = NewObject<USizeBox>(this);
	VBox_KillerCount->AddChild(sizeBox);
	UVerticalBoxSlot* slot = Cast<UVerticalBoxSlot>(sizeBox->Slot);
	slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
}

void ULobbyUI::SetMyPlayerState()
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		myPlayerState = pc->GetPlayerState<ALobbyPlayerState>();
		if (myPlayerState)
		{
			GetWorld()->GetTimerManager().ClearTimer(playerStateHandle);
		}
	}
}

void ULobbyUI::SetPrimaryColor_R(float Value)
{
	Slider_PColor_R->SetValue(Value);
	LobbyPlayerState->SetMeshColor();
}

void ULobbyUI::SetPrimaryColor_G(float Value)
{
	Slider_PColor_G->SetValue(Value);
	LobbyPlayerState->SetMeshColor();
}

void ULobbyUI::SetPrimaryColor_B(float Value)
{
	Slider_PColor_B->SetValue(Value);
	LobbyPlayerState->SetMeshColor();
}

void ULobbyUI::SetSecondaryColor_R(float Value)
{
	Slider_SColor_R->SetValue(Value);
	LobbyPlayerState->SetMeshColor();
}

void ULobbyUI::SetSecondaryColor_G(float Value)
{
	Slider_SColor_G->SetValue(Value);
	LobbyPlayerState->SetMeshColor();
}

void ULobbyUI::SetSecondaryColor_B(float Value)
{
	Slider_SColor_B->SetValue(Value);
	LobbyPlayerState->SetMeshColor();
}
