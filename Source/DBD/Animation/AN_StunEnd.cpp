// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_StunEnd.h"

#include "Character/Killer.h"

void UAN_StunEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	auto* Killer = Cast<AKiller>(MeshComp->GetOwner());
	Killer->bStunned = false;
}
