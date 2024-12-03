﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DBD_Interface_Gimmick.h"
#include "Generator.generated.h"

// 발전기 액터 클래스 -> 생존자의 발전기 애니메이션 상호작용
UCLASS()
class DBD_API AGenerator : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GeneratorMeshComp;

public:	
	// Sets default values for this actor's properties
	AGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction(APawn* Caller = nullptr) override;
	virtual void FailedInteraction() override;
	FORCEINLINE virtual FString GetGimmickName() override;
	FORCEINLINE virtual FString GetInteractKey() override;

	void UpdateRoundGauge();

	inline bool SetIsActivatedSkillCheckZone(bool value) { return IsActivatedSkillCheckZone = value; }

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetGaugeUIVisibility(ESlateVisibility visibility);

	UStaticMeshComponent* GetMeshComp() const { return GeneratorMeshComp; }

public:
	// 발전기 게이지 UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UGaugeUI> GaugeUIWidgetClass;

	UPROPERTY()
	class UGaugeUI* GaugeUI;

	bool IsRoundGauge = false;

	// 발전기 활성화가 됬는지 판단 여부 변수
	UPROPERTY(EditAnywhere, Category = "Generator")
	bool IsActivated = false;


private:
	// 스킬체크 존 활성화 여부
	bool IsActivatedSkillCheckZone = false;

	FString Name = TEXT("Generator");
	FString InteractKey = TEXT("LBM");

	float Percent = 0.0f;
	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsFullGauge = false;
	
	bool GetIsFullGauge() const { return IsFullGauge; }
	void UpdateGauge(float time);

	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SetGaugeUIPercent(float value);
};
