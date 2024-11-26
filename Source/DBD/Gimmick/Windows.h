// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DBD_Interface_Gimmick.h"
#include "Windows.generated.h"


// 창틀 액터 클래스 -> 파쿠르 애니메이션
UCLASS()
class DBD_API AWindows : public AActor, public IDBD_Interface_Gimmick
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WindowFrameMeshComp;

public:	
	// Sets default values for this actor's properties
	AWindows();

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
private:
	FString Name = TEXT("Windows");
	FString InteractKey = TEXT("Space");
};
