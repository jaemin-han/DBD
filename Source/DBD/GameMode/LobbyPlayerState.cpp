// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyPlayerState.h"

#include "GameMode/LobbyGameState.h"
#include "UI/LobbyUI.h"
#include "UI/PlayerCountUI.h"
#include "Net/UnrealNetwork.h"
#include "Components/Button.h"
#include "Components/Image.h"

void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

ALobbyPlayerState::ALobbyPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetUpdateFrequency = 100;
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, PlayerUniqueName);
}

FString ALobbyPlayerState::GetPlayerUniqueName()
{
	return PlayerUniqueName;
}

void ALobbyPlayerState::SetPlayerUniqueName()
{
	AController* controller = Cast<AController>(GetOwner());

	FString name;

	if (controller && controller->HasAuthority() && controller->IsLocalPlayerController())
	{
		name = FString::Printf(TEXT("Killer %d"), GetPlayerId());
	}
	else
	{
		name = FString::Printf(TEXT("Survivor %d"), GetPlayerId());
	}

	Multi_SetPlayerUniqueName(name);
}

void ALobbyPlayerState::OnClickedBtnReady()
{
	ALobbyGameState* lobbyGameState = Cast<ALobbyGameState>(GetWorld()->GetGameState());
	if (lobbyGameState)
	{
		Multi_OnClickedBtnReady(lobbyGameState);
	}
}



void ALobbyPlayerState::Server_OnClickedBtnReady_Implementation()
{
	OnClickedBtnReady();

}
void ALobbyPlayerState::Multi_OnClickedBtnReady_Implementation(ALobbyGameState* lgs)
{
	UE_LOG(LogTemp, Warning, TEXT("[ALobbyPlayerState] Multi_OnClickedBtnReady()"));
	//ULobbyUI* LobbyUI = Cast<ULobbyUI>(lgs->LobbyUI);

	for (auto p : lgs->LobbyUI->allKillerState)
	{
		if (p->LobbyPlayerState == this)
		{
			int32 count = 0;
			for (auto ps : lgs->LobbyUI->allSurvivorState)
			{
				if (ps->LobbyPlayerState->bIsSurvivorReady)
				{
					count++;
				}
			}
			if (count == lgs->LobbyUI->allSurvivorState.Num())
			{
				bIsAllReady = true;
			}
			if (bIsAllReady)
			{
				p->Image_Check->SetColorAndOpacity(FLinearColor::Red);
				if (HasAuthority())
				{
					GetWorld()->ServerTravel(TEXT("/Game/DBD/Level/PlayLevel?listen"));
				}
				break;
			}
			else break;
		}
	}

	for (auto p : lgs->LobbyUI->allSurvivorState)
	{
		if (p->LobbyPlayerState == this)
		{
			p->LobbyPlayerState->bIsSurvivorReady = !p->LobbyPlayerState->bIsSurvivorReady;

			if (p->LobbyPlayerState->bIsSurvivorReady)
			{
				p->Image_Check->SetColorAndOpacity(FLinearColor::Red);
			}
			else
			{
				p->Image_Check->SetColorAndOpacity(FLinearColor::White);
			}

			break;
		}
	}
}




void ALobbyPlayerState::Server_SetPlayerUniqueName_Implementation()
{
	SetPlayerUniqueName();
}

void ALobbyPlayerState::Multi_SetPlayerUniqueName_Implementation(const FString& name)
{
	PlayerUniqueName = name;
}
