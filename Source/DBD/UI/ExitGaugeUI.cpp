// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ExitGaugeUI.h"
#include "Components/ProgressBar.h"
#include "Net/UnrealNetwork.h"
#include "Character/DBD_Player.h"
void UExitGaugeUI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UExitGaugeUI, Percent);
	//DOREPLIFETIME(UExitGaugeUI, ExitGauge);
	//DOREPLIFETIME(UExitGaugeUI, IsFullGauge);
}

void UExitGaugeUI::UpdateExitGauge(float time)
{
	Client_UpdateExitGauge(time);
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
	//ExitGauge->SetPercent(Percent);

	Client_UpdateExitGauge(Percent);
}

void UExitGaugeUI::Client_UpdateExitGauge_Implementation(float time)
{

	if (GetOwningPlayer())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *GetOwningPlayer()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetOwningPlayer is nullptr"));
	}
	Percent = time;
	
	ADBD_Player* player = Cast<ADBD_Player>(GetOwningPlayerPawn());

	if (ExitGauge)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client Percent : %f, %d"), time, player->HasAuthority());
		UE_LOG(LogTemp, Warning, TEXT("Client Gauge Percent : %f"), ExitGauge->GetPercent());
		ExitGauge->SetPercent(time);
	}
}
