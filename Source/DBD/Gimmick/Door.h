﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DBD_Interface_Gimmick.h"
#include "Door.generated.h"

UCLASS()
class DBD_API ADoor : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void Interaction(AActor* Caller = nullptr) override;
	virtual void FailedInteraction() override;

	void OpenExitDoor();

private:
	UPROPERTY(EditAnywhere, Category = "Door")
	class UStaticMeshComponent* ExitMesh;		// 탈출 장치 메쉬

	UPROPERTY(EditAnywhere, Category = "Door")
	class UStaticMeshComponent* DoorMesh;		// 문 메쉬	

	UPROPERTY(EditAnywhere, Category = "Door")
	class UBoxComponent* GaugeCol;				// 게이지 박스

	UPROPERTY(EditAnywhere, Category = "Door")
	class UBoxComponent* ExitCol;				// 탈출 충돌 박스

	UPROPERTY(EditAnywhere, Category = "Door")
	TArray<class AGenerator*> Generators;		// 발전기들

	// 탈출 게이지 UI
	UPROPERTY(EditAnywhere, Category = "Door")
	TSubclassOf<class UExitGaugeUI> ExitGaugeUIClass;

	UPROPERTY()
	class UExitGaugeUI* ExitGaugeUI;


	// 문 활성화 상태판단 여부
	UPROPERTY(EditAnywhere, Category = "Door")
	bool IsDoorActivated = false;
};
