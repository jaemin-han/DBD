// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Killer.h"

#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "Components/ArrowComponent.h"
#include "Gimmick/Hanger.h"
#include "Gimmick/Pallet.h"
#include "Net/UnrealNetwork.h"

void UAI_Killer::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Killer = Cast<AKiller>(TryGetPawnOwner());
	if (Killer)
	{
		Killer->OnHangSurvivor.BindLambda([this](TScriptInterface<IDBD_Interface_Gimmick> Gimmick)
		{
			Hanger = Cast<AHanger>(Gimmick.GetObject());
		});
		Killer->OnDestroyPallet.BindLambda([this](APallet* Gimmick)
		{
			UE_LOG(LogTemp, Warning, TEXT("UAI_Killer::NativeInitializeAnimation - OnDestroyPallet"));
			//  debug Gimmick
			UE_LOG(LogTemp, Warning, TEXT("Gimmick : %s"), *Gimmick->GetName());
			Pallet = Gimmick;
		});
	}
}

void UAI_Killer::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAI_Killer, Pallet);
}

void UAI_Killer::AnimNotify_HangOnHook()
{
	if (Killer == nullptr)
		return;
	ADBD_Player* CarriedSurvivor = Killer->CarriedSurvivor;

	CarriedSurvivor->ChangeSurvivorState(ESurvivorState::Hang);
	CarriedSurvivor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CarriedSurvivor->AttachToComponent(Hanger->HangPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// hanger 에 매달린 생존자 포인터를 CarriedSurvivor로 설정
	Hanger->SetHangSurvivor(CarriedSurvivor);
	Hanger->SetOwner(CarriedSurvivor);

	// 살인마 클래스의 CarriedSurvivor 변수를 nullptr로 초기화
	Killer->CarriedSurvivor = nullptr;

	// EnableInput
	Killer->EnableInput(Cast<APlayerController>(Killer->GetController()));
}

void UAI_Killer::AnimNotify_DestroyPallet()
{
	if (Killer == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("UAI_Killer::AnimNotify_DestroyPallet"));
	if (Pallet == nullptr)
		return;
	Pallet->DestroyPallet();
}
