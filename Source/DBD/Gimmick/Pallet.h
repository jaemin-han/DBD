// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBD_Interface_Gimmick.h"
#include "GameFramework/Actor.h"
#include "Pallet.generated.h"

// 판자 액터 클래스 -> 생존자 : 엎어뜨리는 애니메이션, 살인자 : 기절, 부시는 애니메이션
UCLASS()
class DBD_API APallet : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	UStaticMeshComponent* PalletMeshComp;

	// 판자가 내려간 후, 플레이어가 판자와 겹치는 위치에 있지 않게 하기 위해 위치를 설정한다.
	// front 나 back 중 가까운 위치로 플레이어를 이동시킨다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class USphereComponent* FrontIndicator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class USphereComponent* BackIndicator;

	// 판자가 내려간 후, 플레이어가 판자를 넘어갈 수 없게 보이지 않는 벽을 생성한다
	// 판자를 내린 후에 물리적 충돌이 활성화되고, 플레이어가 판자를 넘어갈 수 없게 된다.
	// 판자가 부서진 후에는 엑터가 제거된다. (혹시 모르니 물리적 충돌을 비활성화한다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class UBoxComponent* WallComp;

	FString Name = TEXT("Pallet");
	FString InteractKey = TEXT("Space");
public:
	// Sets default values for this actor's properties
	APallet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction(AActor* Caller = nullptr) override;
	virtual void FailedInteraction() override;
	virtual FString GetGimmickName() override;
	virtual FString GetInteractKey() override;
	

	// board 가 이미 넘어진 상태인가
	bool bIsFallen = false;
	// 상호작용이 실행되었는가
	bool bIsInteracted = false;
	// 목표 roll 각도
	float TargetRoll;
	// 현재 roll 각도
	float CurrentRoll;

	UFUNCTION()
	void PalletFall();

	UFUNCTION(BlueprintImplementableEvent)
	void DestroyPallet();
};
