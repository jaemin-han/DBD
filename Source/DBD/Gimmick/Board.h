// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBD_Interface_Gimmick.h"
#include "GameFramework/Actor.h"
#include "Board.generated.h"

// 판자 액터 클래스 -> 생존자 : 엎어뜨리는 애니메이션, 살인자 : 기절, 부시는 애니메이션
UCLASS()
class DBD_API ABoard : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BoardMeshComp;

public:	
	// Sets default values for this actor's properties
	ABoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction() override;

	// board 가 이미 넘어진 상태인가
	bool bIsFallen = false;
	// 상호작용이 실행되었는가
	bool bIsInteracted = false;
	// 목표 roll 각도
	float TargetRoll;
	// 현재 roll 각도
	float CurrentRoll;

};
