// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RoundGaugeUI.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void URoundGaugeUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void URoundGaugeUI::UpdatePercent(float per)
{
    if (not IMG_RoundGauge)
    {
        return;
    }

	// Material Instacne 생성하기
	UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(IMG_RoundGauge->Brush.GetResourceObject());
    if (not MI_RoundProcessBar)
    {
        MI_RoundProcessBar = UMaterialInstanceDynamic::Create(MaterialInterface, this);

        IMG_RoundGauge->SetBrushFromMaterial(MI_RoundProcessBar);
    }

	MI_RoundProcessBar->SetScalarParameterValue(TEXT("Percent"), per);
}