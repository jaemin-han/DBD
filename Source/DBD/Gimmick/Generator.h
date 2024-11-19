// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Generator.generated.h"

// 발전기 액터 클래스 -> 생존자의 발전기 애니메이션 상호작용
UCLASS()
class DBD_API AGenerator : public AActor
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
