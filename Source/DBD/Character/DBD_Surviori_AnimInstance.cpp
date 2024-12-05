// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DBD_Surviori_AnimInstance.h"
#include "DBD_Player.h"
#include "Gimmick/DBD_Interface_Gimmick.h"

void UDBD_Surviori_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 내가 붙어있는 Pawn 가져오기
	Player = Cast<ADBD_Player>(TryGetPawnOwner());

	// 구출을 실행할 때 이 함수를 실행함
	//Player->OnSetGimmickAtRescue.BindLambda([this](TScriptInterface<class IDBD_Interface_Gimmick> NearGimmick)
	//{
	//	Gimmick = NearGimmick;
	//});
}

void UDBD_Surviori_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Player)
	{
		//Health = Player->GetHealth();
		Health = (int32)Player->GetSurvivorState();
		IsRunning = Player->GetIsRunning();
		IsCrouching = Player->GetCrouching();
		IsGenerator = Player->GetIsInteractGenerator();
		IsPiggyBack = Player->GetIsPiggyBack();
		IsHang = Player->GetIsHang();

	}
}

void UDBD_Surviori_AnimInstance::AnimNotify_OnParkourFinish()
{
	if (Player)
	{
		Player->Server_FinishParkourFunc();
	}
}

void UDBD_Surviori_AnimInstance::AnimNotify_OnParkour2Finish()
{
	if (Player)
	{
		Player->Server_FinishParkourFunc();
	}
}

void UDBD_Surviori_AnimInstance::AnimNotify_OnRescueFinish()
{
	if (Player)
	{
		if (Gimmick)
		{
			Gimmick->Interaction(Player);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NearGimmick is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is null"));
	}
}
