// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_Parkour.h"

#include "Character/DBDCharacter.h"

void UAN_Parkour::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	auto* player = Cast<ADBDCharacter>(MeshComp->GetOwner());

	if (player)
	{
		player->FinishParkourFunc();
	}
}
