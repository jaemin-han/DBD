// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "Killer.generated.h"

class UDBD_Interface_Gimmick;

DECLARE_DELEGATE_OneParam(FOnHangSurvivorDelegate, TScriptInterface<class IDBD_Interface_Gimmick>);
DECLARE_DELEGATE_OneParam(FOnDestroyPalletDelegate, class APallet*);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DropDownSurvivorAction;

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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// attack function
	void Attack();
	// server attack function
	UFUNCTION(Server, Reliable)
	void ServerRPC_Attack();
	// multicast attack function
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Attack();

	virtual bool IsAttacking() const override { return bIsAttacking; };

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 근처에 있는 생존자를 가져오는 함수
	void GetNearSurvivor();
	// NearSurvivor 를 multicast 로 설정하는 함수
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetNearSurvivor(class ADBD_Player* NewNearSurvivor);
	// 전방에 있는 기믹을 가져오는 함수
	void GetNearGimmick();
	void ShowInteractionUI();
	void SetInteractionUI(bool IsVisible, FString Name, FString Key);
	void Debug();
	// 판자에 맞았을 때 발동되는 함수
	UFUNCTION(NetMulticast, Reliable)
	void Stun();
	// 내려진 판자를 부수는 함수
	UFUNCTION(Server, Reliable)
	void DestroyPallet();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DestroyPallet();
	void Interact();

	// 체력이 0인 생존자를 들처매는 함수
	void CarrySurvivor();
	// server RPC for carry survivor
	UFUNCTION(Server, Reliable)
	void ServerRPC_CarrySurvivor();
	// multicast RPC for carry survivor
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_CarrySurvivor();
	
	void DropDownSurvivor();
	// server RPC for drop down survivor
	UFUNCTION(Server, Reliable)
	void ServerRPC_DropDownSurvivor();
	// multicast RPC for drop down survivor
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DropDownSurvivor();


	// 옮기고 있는 생존자를 갈고리에 거는 함수
	UFUNCTION()
	void HangSurvivorOnHook();
	// server RPC for hang survivor on hook
	UFUNCTION(Server, Reliable)
	void ServerRPC_HangSurvivorOnHook();
	// multicast RPC for hang survivor on hook
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_HangSurvivorOnHook();
	

public:
	// 특정 거리 내에 있는 Gimmick을 가져오는 함수
	// TScriptInterface<class IDBD_Interface_Gimmick> NearGimmick;
	// 가까이에 있는 생존자
	UPROPERTY(VisibleAnywhere)
	class ADBD_Player* NearSurvivor;
	// 옮기고 있는 생존자
	UPROPERTY(VisibleAnywhere)
	class ADBD_Player* CarriedSurvivor;
	// 생존자가 부착될 소켓
	FName CarrySocketName = "CarrySocket";
	// 킬러가 스턴 상태인가요?
	bool bStunned = false;
	// 킬러가 공격 중인가요?
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, Category = "GameState")
	class ADBDGameState* DBDGameState;

	FOnHangSurvivorDelegate OnHangSurvivor;
	FOnDestroyPalletDelegate OnDestroyPallet;

private:
	// attack montage
	UPROPERTY(EditAnywhere, Category = Animation, meta = (AllowPrivate))
	UAnimMontage* KillerMontage;
	// InteractionUI 추가
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInteractionUI> InteractionUIClass; // 이후 플레이 UI가 추가된다면 InteractionUI 클래스로 변경
	UPROPERTY()
	class UInteractionUI* InteractionUI;
};
