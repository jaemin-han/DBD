// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_DestroyBoard.h"

#include "Character/Killer.h"
#include "Gimmick/Board.h"

class AKiller;

void UAN_DestroyBoard::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AKiller* Killer = Cast<AKiller>(MeshComp->GetOwner()))
	{
		if (ABoard* Board = Cast<ABoard>(Killer->NearGimmick.GetObject()))
		{
			Board->DestroyBoard();
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
