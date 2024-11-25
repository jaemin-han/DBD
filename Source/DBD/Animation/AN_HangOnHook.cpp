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
	CarriedSurvivor = nullptr;
}
