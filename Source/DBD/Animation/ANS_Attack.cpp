// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_Attack.h"

#include "Character/DBD_Player.h"

void UANS_Attack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                             const FAnimNotifyEventReference& EventReference)
{
	// 이미 공격이 발생했었으면 더 이상 공격을 처리하지 않음
	if (bHasValidHitOccurred)
		return;
	
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	PerformSphereTrace(MeshComp);	
}

void UANS_Attack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// 공격이 끝났을 때, 다음 공격을 위해 변수를 초기화
	bHasValidHitOccurred = false;
}

FHitResult UANS_Attack::PerformSphereTrace(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return FHitResult();

	FVector StartLocation = MeshComp->GetSocketLocation(SocketStart);
	FVector EndLocation = MeshComp->GetSocketLocation(SocketEnd);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());

	FHitResult HitResult;
	float SphereRadius = 50.0f;

	bool bHit = MeshComp->GetWorld()->SweepSingleByProfile(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		FName("KillerWeapon"),
		FCollisionShape::MakeSphere(SphereRadius),
		Params
		);

	if (bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Sphere Trace hit: %s"), *HitResult.GetActor()->GetName());

		// HitResult 의 결과가 ADBD_Player 인 경우, 플레이어에게 데미지를 입히는 로직을 추가
		auto* Survivor =Cast <ADBD_Player>(HitResult.GetActor());
		if (Survivor)
		{
			// Todo: 플레이어에게 데미지를 입히는 로직 추가, 승호님과 상의 필요
			Survivor->UpdateHP(1);
			// 중복 공격을 막기 위해 유효한 공격이 발생했음을 표시
			bHasValidHitOccurred = true;
		}
		// Optional: Debug visualization
		DrawDebugSphere(MeshComp->GetWorld(), HitResult.ImpactPoint, 2.0f, 12, FColor::Green, false, 0.5f);
	}
	else
	{
		// Optional: Debug visualization for the trace
		DrawDebugLine(MeshComp->GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.5f, 0, 2.0f);
	}

	return HitResult;
}
