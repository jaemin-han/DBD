// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DBD_Surviori_AnimInstance.h"
#include "DBD_Player.h"

void UDBD_Surviori_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// ���� �پ��ִ� Pawn ��������
	Player = Cast<ADBD_Player>(TryGetPawnOwner());
}

void UDBD_Surviori_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Player)
	{
		Health = Player->GetHealth();
		IsRunning = Player->GetIsRunning();
		IsCrouching = Player->GetCrouching();
	}
}
