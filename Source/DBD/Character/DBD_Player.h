// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "DBD_Player.generated.h"

/**
 * 
 */
UCLASS()
class DBD_API ADBD_Player : public ADBDCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void PlusHp();
	void MinusHp();
	void Run();
	void RunStop();
	void Crouch();
	void CrouchStop();
public:
	// �������� HP ������Ʈ �Լ�
	void UpdateHP(int32 Damage);
	void UpdateSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline int32 GetHealth() const {return Health;}
	inline bool GetIsRunning() const {return IsRunning;}
	inline bool GetCrouching() const {return IsCrouching;}

private:
	// �ӽÿ� ü�� ���� Ű��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PlusHpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MinusHpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;



	// ������ ü��
	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 Health; // 3 : ���� 2 : �λ� 1 : �߻� 0 : ����

	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 MaxHealth = 3; 

	// �ٴ��� �ȶٴ��� üũ ����
	bool IsRunning = false;
	// Crouch �������� üũ ����
	bool IsCrouching = false;
};