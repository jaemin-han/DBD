// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_AttackEnd.h"

#include "Character/Killer.h"

void UAN_AttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	auto* Killer = Cast<AKiller>(MeshComp->GetOwner());

	if (Killer)
	{
		Killer->bIsAttacking = false;
	}
}
