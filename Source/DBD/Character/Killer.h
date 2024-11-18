// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "Killer.generated.h"

UCLASS()
class DBD_API AKiller : public ADBDCharacter
{
	GENERATED_BODY()

private:
	// attack input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

private:
	// attack montage
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivate))
	UAnimMontage* AttackMontage;

	// weapon static mesh component
	UPROPERTY(EditAnywhere, Category = Weapon, meta = (AllowPrivate))
	UStaticMeshComponent* Weapon;

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
};
