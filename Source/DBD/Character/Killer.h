// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "Killer.generated.h"

class UDBD_Interface_Gimmick;

UCLASS()
class DBD_API AKiller : public ADBDCharacter
{
	GENERATED_BODY()

private:
	// attack input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	// interaction input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;

private:
	// attack montage
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivate))
	UAnimMontage* KillerMontage;

	// weapon static mesh component
	UPROPERTY(EditAnywhere, Category = Weapon, meta = (AllowPrivate))
	UStaticMeshComponent* Weapon;

	// sphere collision component for search gimmick
	UPROPERTY(EditAnywhere, Category = Gimmick, meta = (AllowPrivate))
	class USphereComponent* SearchGimmickSphere;

public:
	// Sets default values for this character's properties
	AKiller();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// attack function
	void Attack();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 특정 거리 내에 있는 Gimmick을 가져오는 함수
	void GetNearGimmick();
	TScriptInterface<class IDBD_Interface_Gimmick> NearGimmick;

	void Debug();
	void Stun();
	void Interact();
};
