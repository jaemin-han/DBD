// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GaugeUI.h"

#include "Components/ProgressBar.h"

void UGaugeUI::UpdateGauge(float time)
{
	// time = 0.2로 들어오는데 이거를 100퍼로 나눴을때
	// 초당 0.01씩 증가하게 만들기 위해서
	Percent += time * 0.011f;


	Gauge->SetPercent(Percent);
}