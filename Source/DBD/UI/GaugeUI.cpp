// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GaugeUI.h"

#include "Components/ProgressBar.h"
#include "RoundGaugeUI.h"

void UGaugeUI::NativeConstruct()
{
	Super::NativeConstruct();

	//RoundGauge = Cast<URoundGaugeUI>(GetWidgetFromName(TEXT("RoundGaugeUI")));
}

void UGaugeUI::UpdateGauge(float time)
{
	if (Percent >= 1.0f)
	{
		IsFullGauge = true;
		return;
	}
	
	// time = 0.2로 들어오는데 이거를 100퍼로 나눴을때
	// 초당 0.01씩 증가하게 만들기 위해서
	//Percent += time * 0.011f;		// 실제 적용
	Percent += time * 0.5f;			// 테스트용


	Gauge->SetPercent(Percent);
}

bool UGaugeUI::UpdateRoundPercent(float time)
{
	if (!RoundGauge) return false;
	// time = 0.2로 들어오는데 이거를 100퍼로 나눴을때
	// 초당 33씩 증가하게 만들기 위해서
	RoundGauge->Percent += time * 0.5f;

	// float percent += 100 * DeltaTime;

	RoundGauge->UpdatePercent();

	UE_LOG(LogTemp, Log, TEXT("UpdateRondPercent"));
	// 다 찼거나, 플레이어가 스페이스바를 올바르게 눌렀을 때
	if (RoundGauge->Percent >= 1.0f)
	{
		RoundGauge->Percent = 0.0f;
		return true;
	}
	else return false;
}

void UGaugeUI::VisibleRondGauge(bool IsVisible)
{
	if (!RoundGauge)
	{
		UE_LOG(LogTemp, Warning, TEXT("RoundGauge is nullptr"));
		return;
	}

	if (IsVisible)
	{
		UE_LOG(LogTemp, Log, TEXT("VisibleRondGauge"));
		RoundGauge->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("hiddenRondGauge"));
		RoundGauge->SetVisibility(ESlateVisibility::Hidden);
		RoundGauge->Percent = 0.0f;
	}
}