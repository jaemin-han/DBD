// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_Attack.h"

void UANS_Attack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	PerformSphereTrace(MeshComp);	
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

	bool bHit = MeshComp->GetWorld()->SweepSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SphereRadius),
		Params
		);

	if (bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Sphere Trace hit: %s"), *HitResult.GetActor()->GetName());

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
