// Fill out your copyright notice in the Description page of Project Settings.


#include "HangerUI.h"

#include "Components/ProgressBar.h"

void UHangerUI::SetSacrifaceGagePercent(const float Percent) const
{
	SacrifaceGage->SetPercent(Percent);
}
