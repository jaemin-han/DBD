// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_HangOnHook.h"

#include "Character/DBD_Player.h"
#include "Character/Killer.h"
#include "Components/ArrowComponent.h"
#include "Gimmick/Hanger.h"

void UAN_HangOnHook::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AKiller* Killer = Cast<AKiller>(MeshComp->GetOwner());
	if (Killer == nullptr)
		return;
	ADBD_Player* CarriedSurvivor = Killer->CarriedSurvivor;
	AHanger* Hanger = Cast<AHanger>(Killer->NearGimmick.GetObject());

	CarriedSurvivor->ChangeSurvivorState(ESurvivorState::Hang);
	CarriedSurvivor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CarriedSurvivor->AttachToComponent(Hanger->HangPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// hanger 에 매달린 생존자 포인터를 CarriedSurvivor로 설정
	Hanger->SetHangSurvivor(CarriedSurvivor);
	Hanger->SetOwner(CarriedSurvivor);
	
	// 살인마 클래스의 CarriedSurvivor 변수를 nullptr로 초기화
	Killer->CarriedSurvivor = nullptr;
}
