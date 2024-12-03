// Fill out your copyright notice in the Description page of Project Settings.

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
	FORCEINLINE virtual FString GetGimmickName() override;
	FORCEINLINE virtual FString GetInteractKey() override;

	UFUNCTION(Server, Reliable)
	void Server_InteractDoor(AActor* Caller = nullptr);	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetOwner(AActor* Caller);
	UFUNCTION(Client, Reliable)
	void Client_InteractDoor(AActor* Caller = nullptr);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_InteractDoor(AActor* Caller = nullptr);

	UFUNCTION(Client, Reliable)
	void Client_FailedInterat();


	void OpenExitDoor();
	UFUNCTION(Server, Reliable)
	void Server_OpenExitDoor();
	UFUNCTION(NetMulticast,Reliable)
	void Multi_OpenExitDoor();


	// UI 게이지 업데이트
	void UpdateExitGauge();

	UFUNCTION(Client, Reliable)
	void Client_UpdateExitGauge(float persent);






	class UExitGaugeUI* GetExitGaugeUI() { return ExitGaugeUI; } 

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

	//UPROPERTY(Replicated)
	class UExitGaugeUI* ExitGaugeUI;
	
	FString Name = TEXT("Door");
	FString InteractKey = TEXT("Space");

	// 문 활성화 상태판단 여부
	UPROPERTY(EditAnywhere, Category = "Door")
	bool IsDoorActivated = false;
};
