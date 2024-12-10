// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SkillCheckZoneUI.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void USkillCheckZoneUI::UpdatePercent(float percent)
{
    if (not IMG_SkillCheckZone)
    {
        return;
    }

    // Material Instacne 생성하기
    UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(IMG_SkillCheckZone->GetBrush().GetResourceObject());
    if (not MI_SkillCheckZone)
    {
        MI_SkillCheckZone = UMaterialInstanceDynamic::Create(MaterialInterface, this);

        IMG_SkillCheckZone->SetBrushFromMaterial(MI_SkillCheckZone);
    }

    MI_SkillCheckZone->SetScalarParameterValue(TEXT("Percent"), Percent);
}

void USkillCheckZoneUI::SetPercent(float percent)
{
    if (not IMG_SkillCheckZone)
    {
        return;
    }
    UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(IMG_SkillCheckZone->GetBrush().GetResourceObject());
    if (not MI_SkillCheckZone)
    {
        MI_SkillCheckZone = UMaterialInstanceDynamic::Create(MaterialInterface, this);

        IMG_SkillCheckZone->SetBrushFromMaterial(MI_SkillCheckZone);
    }

    MI_SkillCheckZone->SetScalarParameterValue(TEXT("Percent"), percent);
}
