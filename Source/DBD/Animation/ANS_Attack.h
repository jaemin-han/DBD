// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_Attack.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API UANS_Attack : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	FName SocketStart = "WeaponSocket_l";
	FName SocketEnd = "WeaponSocket_l_End";

protected:
	FHitResult PerformSphereTrace(USkeletalMeshComponent* MeshComp);
};
