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
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	FName SocketStart = "WeaponSocket_l";
	FName SocketEnd = "WeaponSocket_l_End";

protected:
	FHitResult PerformSphereTrace(USkeletalMeshComponent* MeshComp);

	// 중복 공격을 막기 위해 유효한 공격이 발생했었는지를 나타내는 변수
	bool bHasValidHitOccurred = false;
};
