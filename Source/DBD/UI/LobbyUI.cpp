// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyUI.h"

#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"

#include "PlayerCountUI.h"
#include "GameMode/LobbyPlayerState.h"



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


	Image_Player->OnMouseButtonDownEvent.BindUFunction(this, "OnClickedImagePlayer");


	// Btn_Ready이 눌렸을때 호출되는 함수
	Btn_Ready->OnClicked.AddDynamic(this, &ULobbyUI::OnClickedBtnReady);

	// 나의 PlayerState 찾아오는 Timer 돌리자
	GetWorld()->GetTimerManager().SetTimer(playerStateHandle, this, &ULobbyUI::SetMyPlayerState, 0.1f, true);
}

void ULobbyUI::OnClickedBtnReady()
{
	UE_LOG(LogTemp, Warning, TEXT("[LobbyUI] Btn_Ready Clicked"));
}

void ULobbyUI::OnClickedImagePlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("[LobbyUI] Image_Player Clicked"));

	bIsClickedImagePlayer = !bIsClickedImagePlayer;

	if (bIsClickedImagePlayer)
	{
		Canvas_PlayerCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Canvas_PlayerCount->SetVisibility(ESlateVisibility::Hidden);
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

