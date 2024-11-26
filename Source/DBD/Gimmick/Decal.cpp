// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/Decal.h"

ADecal::ADecal()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADecal::BeginPlay()
{
	Super::BeginPlay();

	// Material Instacne 생성하기
	UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(GetDecalMaterial());
	MI_Decal = UMaterialInstanceDynamic::Create(MaterialInterface, this);
	SetDecalMaterial(MI_Decal);

	// 2초 후에 삭제
	FTimerHandle timeHandle;
	GetWorld()->GetTimerManager().SetTimer(timeHandle, this, &ADecal::DestroyDecal, 5.0f, false);
}

void ADecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	DestroyTimer += DeltaTime;

	//UE_LOG(LogTemp, Warning, TEXT("Decal DestroyTimer : %f"), DestroyTimer);
	if (DestroyTimer > 1.0f)
	{
		Percent -= DeltaTime / 3.0f;
		//UE_LOG(LogTemp, Warning, TEXT("Decal Percent : %f"), Percent);
		MI_Decal->SetScalarParameterValue("Opacity", Percent);
	}
}

void ADecal::DestroyDecal()
{
	Destroy();
}