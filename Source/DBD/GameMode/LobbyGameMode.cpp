// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

#include "DBDGameInstance.h"
#include "Character/DBD_Player.h"
#include "Character/Killer.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 화면에 PlayerCount 출력해주기
	
}

APawn* ALobbyGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	//Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);


	PlayerCount++;
	UE_LOG(LogTemp, Warning, TEXT("PlayerCount : %d"), PlayerCount);
	UE_LOG(LogTemp, Warning, TEXT("SpawnDefaultPawnFor_Implementation"));

	StartSpot = ChoosePlayerStart(NewPlayer);
	//UE_LOG(LogTemp, Warning, TEXT("startSpot pos X : %.2f, Y : %.2f, Z : %.2f"), StartSpot->GetActorLocation().X, StartSpot->GetActorLocation().Y, StartSpot->GetActorLocation().Z);
	//UE_LOG(LogTemp, Warning, TEXT("startSpot rot roll : %.2f, pitch : %.2f, yaw : %.2f"), StartSpot->GetActorRotation().Roll, StartSpot->GetActorRotation().Pitch, StartSpot->GetActorRotation().Yaw);
	if (NewPlayer->IsLocalController())
	{
		return GetWorld()->SpawnActor<AKiller>(KillerClass, 
			StartSpot->GetActorLocation(),
			StartSpot->GetActorRotation());
	}
	else
	{
		return GetWorld()->SpawnActor<ADBD_Player>(SurvivorClass, 
			StartSpot->GetActorLocation(),
			StartSpot->GetActorRotation());
	}
}

AActor* ALobbyGameMode::ChoosePlayerStart(AController* Player)
{
	static int32 num = 1;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* playerStart = *It;
		FString tag = "Survivor" + FString::FromInt(num);

		if (playerStart->PlayerStartTag == "Killer")
		{
			if (Player->IsLocalController())
			{
				return playerStart;
			}
			else continue;
		}
		else if (playerStart->PlayerStartTag == tag)
		{
			if (Player->IsLocalController())
			{
				continue;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("playerStart[%s] pos X : %.2f, Y : %.2f, Z : %.2f"), *tag, playerStart ->GetActorLocation().X, playerStart->GetActorLocation().Y, playerStart->GetActorLocation().Z);
				UE_LOG(LogTemp, Warning, TEXT("playerStart[%s] rot roll : %.2f, pitch : %.2f, yaw : %.2f"), *tag, playerStart->GetActorRotation().Roll, playerStart->GetActorRotation().Pitch, playerStart->GetActorRotation().Yaw);
				num++;
				return playerStart;
			}
		}
		
		//if(Player->IsLocalController())
		//{
		//	if (playerStart->PlayerStartTag == "Killer")
		//	{
		//		return playerStart;
		//	}
		//}
		//else //if (!Player->IsLocalController())
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Local ChoosePlayerStart"));
		//	FString tag = "Survivor" + FString::FromInt(num);
		//	if (playerStart->PlayerStartTag == tag)
		//	{
		//		UE_LOG(LogTemp, Warning, TEXT("playerStart[%s] pos X : %.2f, Y : %.2f, Z : %.2f"), *tag, playerStart-//>GetActorLocation().X, playerStart->GetActorLocation().Y, playerStart->GetActorLocation().Z);
		//		UE_LOG(LogTemp, Warning, TEXT("playerStart[%s] rot roll : %.2f, pitch : %.2f, yaw : %.2f"), *tag, //playerStart->GetActorRotation().Roll, playerStart->GetActorRotation().Pitch, playerStart-/>GetActorRotation/().Yaw);
		//		num++;
		//		return playerStart;
		//	}
		//	//return Super::ChoosePlayerStart(Player);
		//}
	}
	
	return Super::ChoosePlayerStart(Player);
}
