// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ExitGaugeUI.h"
#include "Components/ProgressBar.h"

void UExitGaugeUI::UpdateExitGauge(float time)
{
	if (Percent > 1.0f)
	{
		IsFullGauge = true;
		return;
	}


	// 초당 0.0625씩 증가하게 만들기 위해서 -> 총 약 16초
	//Percent += time *  0.0625;		// 실제 게임용
	Percent += time *  0.625;			// 테스트용
	ExitGauge->SetPercent(Percent);
}
