// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DBD_Surviori_AnimInstance.h"
#include "DBD_Player.h"
#include "Gimmick/DBD_Interface_Gimmick.h"
#include "Kismet/GameplayStatics.h"

void UDBD_Surviori_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 내가 붙어있는 Pawn 가져오기
	Player = Cast<ADBD_Player>(TryGetPawnOwner());

	// 구출을 실행할 때 이 함수를 실행함
	if (Player)
	{
		Player->OnSetGimmickAtRescue.BindLambda([this](TScriptInterface<IDBD_Interface_Gimmick> NearGimmick)
	   {
		   Gimmick = NearGimmick;
	   });
	}
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

void UDBD_Surviori_AnimInstance::AnimNotify_OnFootStep()
{
	if (Player)
	{
		// killer 에서 z 방향으로 -200 만큼 line trace 를 진행해서, 땅에 닿은 경우 발자국 소리를 출력
		FVector Start = Player->GetActorLocation();
		FVector End = Start - FVector(0, 0, 200);
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Player);
		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams))
		{
			float RandomPitch = FMath::RandRange(0.9f, 1.2f);

			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Player->FootStepSound, HitResult.Location, 1, RandomPitch, 0, Player->FootStepAttenuation);
		}
	}
}
