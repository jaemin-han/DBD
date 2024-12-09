// Fill out your copyright notice in the Description page of Project Settings.


#include "DBDGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include <Online/OnlineSessionNames.h>

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UDBDGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("GameInstance Init"));

	IOnlineSubsystem* Subsys = IOnlineSubsystem::Get();
	if (Subsys)
	{
		SessionInterface = Subsys->GetSessionInterface();
		// todo: 기타 delegate 처리
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDBDGameInstance::OnCreateSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
			this, &UDBDGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UDBDGameInstance::OnFindSessionsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UDBDGameInstance::OnJoinSessionComplete);
	}
	
	SetIsKiller(false);
	SetKillerKillCount(0);
	SetIsEscaped(false);
}

void UDBDGameInstance::CreateSession(FString DisplayName, int32 PlayerCount)
{
	// session option
	FOnlineSessionSettings SessionSettings;

	FName SubsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	UE_LOG(LogTemp, Warning, TEXT("Subsystem name:  %s"), *SubsysName.ToString());

	// LAN
	SessionSettings.bIsLANMatch = SubsysName.IsEqual("NULL");

	// Lobby 를 사용할지 여부
	SessionSettings.bUseLobbiesIfAvailable = true;

	// session 검색을 허용할지 여부
	SessionSettings.bShouldAdvertise = true;

	// session 이 진행 중일 때 참여 가능 여부
	SessionSettings.bAllowJoinInProgress = true;

	// session 의 최대 인원 설정
	SessionSettings.NumPublicConnections = PlayerCount;

	// Custom 정보
	SessionSettings.Set(TEXT("DP_NAME"), DisplayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// session 생성
	// FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	// SessionInterface->CreateSession(*NetID, FName(DisplayName), SessionSettings);

	// debug 용 코드
	// Get the world
	UWorld* World = GetWorld();
	if (World)
	{
		// Get the first local player from the controller
		ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			// Get the unique net ID for the platform user
			FUniqueNetIdRepl UniqueNetIdRepl = LocalPlayer->GetUniqueNetIdForPlatformUser();
			if (UniqueNetIdRepl.IsValid())
			{
				// Get the unique net ID
				FUniqueNetIdPtr NetID = UniqueNetIdRepl.GetUniqueNetId();
				if (NetID.IsValid())
				{
					// Create the session
					SName = DisplayName;
					SessionInterface->CreateSession(*NetID, FName(DisplayName), SessionSettings);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to get valid unique net ID"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get valid unique net ID repl"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get local player from controller"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get world"));
	}
}

void UDBDGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Session created successfully"), *SessionName.ToString());
		// todo: listen server
		//GetWorld()->ServerTravel(TEXT("/Game/DBD/Level/PlayLevel?listen"));
		GetWorld()->ServerTravel(TEXT("/Game/DBD/Level/LobbyLevel?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Session created failed"), *SessionName.ToString());
	}
}

void UDBDGameInstance::DestroySession(FString SessionName)
{
	SessionInterface->DestroySession(FName(SessionName));
}

void UDBDGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Session destroyed successfully"), *SessionName.ToString());
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("SeqLevel"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Session destroyed failed"), *SessionName.ToString());
	}
}

void UDBDGameInstance::FindOtherSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("Find Session Start"));

	// session 검색 설정
	SessionSearch = MakeShared<FOnlineSessionSearch>();

	// LAN 을 사용할지 여부
	FName SubsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	UE_LOG(LogTemp, Warning, TEXT("Subsystem name:  %s"), *SubsysName.ToString());
	SessionSearch->bIsLanQuery = SubsysName.IsEqual("NULL");

	// 활성화 되어 있는 session 만 검색
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// session 을 몇 개까지 검색할지 설정
	SessionSearch->MaxSearchResults = 20;

	// session 검색
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UDBDGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		for (int32 Index = 0; Index < SearchResults.Num(); ++Index)
		{
			FOnlineSessionSearchResult& SearchResult = SearchResults[Index];

			// DP_NAME
			FString DisplayName;
			SearchResult.Session.SessionSettings.Get(TEXT("DP_NAME"), DisplayName);

			// session 을 만든 사람 이름
			FString HostName = SearchResult.Session.OwningUserName;

			// DP_NAME, HostName 출력
			UE_LOG(LogTemp, Warning, TEXT("Session Name: %s, Host: %s"), *DisplayName, *HostName);

			FString Info = FString::Printf(TEXT("%s(%s)"), *DisplayName, *HostName);

			OnAddSession.ExecuteIfBound(Index, Info);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Find Session Success"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Session Failed"));
	}
}

void UDBDGameInstance::JoinOtherSession(int32 Index)
{
	TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

	FString DisplayName;
	SName = DisplayName;
	SearchResults[Index].Session.SessionSettings.Get(TEXT("DP_NAME"), DisplayName);

	SessionInterface->JoinSession(0, FName(DisplayName), SearchResults[Index]);
}

void UDBDGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString URL;
		SessionInterface->GetResolvedConnectString(SessionName, URL);

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		PlayerController->ClientTravel(URL, TRAVEL_Absolute);
	}
}

