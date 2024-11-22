// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RoundGaugeUI.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void URoundGaugeUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void URoundGaugeUI::UpdatePercent()
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


	//MI_RoundProcessBar->ScalarParameterValues;
	MI_RoundProcessBar->SetScalarParameterValue(TEXT("Percent"), Percent);

    //if (!RoundGauge)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("RoundGauge is not initialized!"));
    //    return;
    //}
    //
    //// 브러시 리소스를 가져오기
    //UObject* BrushResource = RoundGauge->Brush.GetResourceObject();
    //if (!BrushResource) // BrushResource 예외처리
    //{
    //    UE_LOG(LogTemp, Error, TEXT("RoundGauge Brush does not have a valid resource!"));
    //    return;
    //}
    //
    //// UMaterialInterface로 캐스팅
    //UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(BrushResource);
    //if (!MaterialInterface) // MaterialInterface 예외처리
    //{
    //    UE_LOG(LogTemp, Error, TEXT("Brush resource is not a material!"));
    //    return;
    //}
    //
    //// Dynamic Material Instance 생성
    //if (!MI_RoundProcessBar)
    //{
    //    MI_RoundProcessBar = UMaterialInstanceDynamic::Create(MaterialInterface, this);
    //    if (!MI_RoundProcessBar) // MI_RoundProcessBar 예외처리
    //    {
    //        UE_LOG(LogTemp, Error, TEXT("Failed to create MaterialInstanceDynamic!"));
    //        return;
    //    }
    //    RoundGauge->SetBrushFromMaterial(MI_RoundProcessBar);
    //}
    //
    //// Percent 값 업데이트
    //MI_RoundProcessBar->SetScalarParameterValue(TEXT("Percent"), Percent);
}