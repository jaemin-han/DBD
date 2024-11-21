// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_DestroyPallet.h"

#include "Character/Killer.h"
#include "Gimmick/Pallet.h"

class AKiller;

void UAN_DestroyPallet::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AKiller* Killer = Cast<AKiller>(MeshComp->GetOwner()))
	{
		if (APallet* Board = Cast<APallet>(Killer->NearGimmick.GetObject()))
		{
			Board->DestroyPallet();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UAN_DestroyBoard::Notify - Failed to cast ABoard"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UAN_DestroyBoard::Notify - Failed to cast AKiller"));
	}
}
