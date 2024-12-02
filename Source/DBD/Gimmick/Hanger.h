// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBD_Interface_Gimmick.h"
#include "GameFramework/Actor.h"
#include "Hanger.generated.h"

class ADBD_Player;
UCLASS()
class DBD_API AHanger : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	FString Name = TEXT("Hanger");
	FString InteractKey = TEXT("Space");

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UArrowComponent* HangPosition;

public:
	// Sets default values for this actor's properties
	AHanger();

	// get HangSurvivor
	class ADBD_Player* GetHangSurvivor() const { return HangSurvivor; }
	// set HangSurvivor
	void SetHangSurvivor(class ADBD_Player* InHangSurvivor);

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

private:
	// 갈고리에 매달린 생존자 포인터
	UPROPERTY(VisibleAnywhere)
	class ADBD_Player* HangSurvivor;

	// todo: HangSurvivor 에게 보여줄 남은 시간 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UHangerUI> HangerUIClass;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UHangerUI* HangerUI;

private:
	void Rescue();
};
