// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GaugeUI.h"

#include "Components/ProgressBar.h"
#include "RoundGaugeUI.h"
#include "SkillCheckZoneUI.h"

#include "Kismet/GameplayStatics.h"
#include "Character/DBD_Player.h"

void UGaugeUI::NativeConstruct()
{
	Super::NativeConstruct();

	//RoundGauge = Cast<URoundGaugeUI>(GetWidgetFromName(TEXT("RoundGaugeUI")));
	
	//Player = Cast<ADBD_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}

void UGaugeUI::SetPercent(float value)
{
	if (!Gauge) return;

	Gauge->SetPercent(value);
}


//bool UGaugeUI::UpdateRoundPercent(float time)
//{
//	if (!RoundGauge) return false;
//	if (not ActivatedRoundGauge) return false;
//
//
//	// time = 0.2로 들어오는데 이거를 100퍼로 나눴을때
//	// 초당 33씩 증가하게 만들기 위해서
//	RoundGauge->Percent += time * 1.0f;
//
//
//	// float percent += 100 * DeltaTime;
//	RoundGauge->UpdatePercent();
//
//	// 스킬체크 구역의 끝지점 SkillCheckZoneStart는 시작 퍼센트지점, Random은 스킬체크 구역 범위
//	float end = SkillCheckZoneStart + Random;
//
//	// 플레이어가 할당되지않았다면 한번 할당해줘
//	if (not Player)
//	{
//		Player = Cast<ADBD_Player>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//	}
//
//	// 게이지가 스킬체크 구역에 들어왔을때
//	if (RoundGauge->Percent >= SkillCheckZoneStart and RoundGauge->Percent <= end)
//	{
//		//UE_LOG(LogTemp, Log, TEXT("SkillCheckZoneStart %f"), SkillCheckZoneStart);
//		if (Player->GetIsSpaceBar())
//		{
//			IsSuccessSkillCheck = true;
//		}
//	}
//
//	//UE_LOG(LogTemp, Log, TEXT("UpdateRondPercent"));
//	// 다 찼거나, 플레이어가 스페이스바를 올바르게 눌렀을 때
//	if (Player->GetIsSpaceBar() and IsSuccessSkillCheck)
//	{
//		RoundGauge->Percent = 0.0f;
//		IsSuccessSkillCheck = false;
//		return true;
//	}
//	// 게이지가 다 찼거나, 플레이거 스페이스바를 눌렀는데 스킬체크에 실패했을때
//	else if (RoundGauge->Percent >= 1.0f or (Player->GetIsSpaceBar() and IsSuccessSkillCheck == false))
//	{
//		IsSuccessSkillCheck = false;		// 스킬체크성공여부는 다시 false로
//		RoundGauge->Percent = 0.0f;			// 게이지 초기화
//		if (Percent <= 0.02f)				// 게이지가 0.02보다 작다면 그냥 0으로 초기화
//		{
//			Percent = 0;
//		}
//		else Percent -= 0.02f;
//		return true;
//	}
//	else return false;
//}
//
//void UGaugeUI::VisibleRondGauge(bool IsVisible)
//{
//	if (!RoundGauge)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("RoundGauge is nullptr"));
//		return;
//	}
//
//	if (IsVisible)
//	{
//		//UE_LOG(LogTemp, Log, TEXT("VisibleRondGauge"));
//		RoundGauge->SetVisibility(ESlateVisibility::Visible);
//	}
//	else
//	{
//		//UE_LOG(LogTemp, Log, TEXT("hiddenRondGauge"));
//		RoundGauge->SetVisibility(ESlateVisibility::Hidden);
//		SkillCheckZone->SetVisibility(ESlateVisibility::Hidden);
//		RoundGauge->Percent = 0.0f;
//	}
//}
//
//inline void UGaugeUI::SetSkillCheckZone(float value)
//{
//	SkillCheckZoneStart = value;
//
//	//UE_LOG(LogTemp, Log, TEXT("value %f"), value);
//	
//	Random = FMath::RandRange(0.1f, 0.2f);
//	//UE_LOG(LogTemp, Log, TEXT("Random %f"), Random);
//	SkillCheckZone->SetPercent(Random);
//
//	// 0 ~ 1의 범위를 -180 ~ 180의 범위로 변환
//	Start = ((value * 2) - 1) * 180.0f;
//	// SKillCheckZone의 RenderTransform의 Angle을 변경
//	//SkillCheckZone->RenderTransform.Angle = start;
//
//	SkillCheckZone->SetVisibility(ESlateVisibility::Visible);
//	FWidgetTransform newTransform = SkillCheckZone->RenderTransform;
//	newTransform.Angle = Start;
//	SkillCheckZone->SetRenderTransform(newTransform);
//
//	SkillCheckZone->InvalidateLayoutAndVolatility();
//	//SkillCheckZone->Invalidate(EInvalidateWidget::LayoutAndVolatility);
//
//	//UE_LOG(LogTemp, Log, TEXT("start %f"), Start);
//	//UE_LOG(LogTemp, Log, TEXT("SkillCheckZone->RenderTransform.Angle %f"), SkillCheckZone->RenderTransform.Angle);
//}
