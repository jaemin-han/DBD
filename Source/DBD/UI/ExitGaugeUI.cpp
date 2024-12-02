// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ExitGaugeUI.h"
#include "Components/ProgressBar.h"
#include "Net/UnrealNetwork.h"

void UExitGaugeUI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UExitGaugeUI, Percent);
	DOREPLIFETIME(UExitGaugeUI, IsFullGauge);
}

void UExitGaugeUI::UpdateExitGauge(float time)
{
	//if (Percent > 1.0f)
	//{
	//	IsFullGauge = true;
	//	return;
	//}
	//
	//UE_LOG(LogTemp, Warning, TEXT("Percent : %f"),  Percent);
	//
	//// 초당 0.0625씩 증가하게 만들기 위해서 -> 총 약 16초
	////Percent += time *  0.0625;		// 실제 게임용
	//Percent += time *  0.625;			// 테스트용
	UE_LOG(LogTemp, Warning, TEXT("Client Percent : %f"), time);
	ExitGauge->SetPercent(time);

	//Client_UpdateExitGauge(time);
}

void UExitGaugeUI::Server_UpdateExitGauge_Implementation()
{
	if (Percent > 1.0f)
	{
		IsFullGauge = true;
		UE_LOG(LogTemp, Warning, TEXT("Server_ IsFullGauge : %d"), IsFullGauge);
		return;
	}

	Percent += GetWorld()->GetDeltaSeconds() * 0.625;
	UE_LOG(LogTemp, Warning, TEXT("Server_ Percent : %f"), Percent);
	UpdateExitGauge(Percent);
}

void UExitGaugeUI::Client_UpdateExitGauge_Implementation(float time)
{
	UE_LOG(LogTemp, Warning, TEXT("Client Percent : %f"), Percent);
	ExitGauge->SetPercent(time);
}
