// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "DBD_Player.generated.h"


UENUM(BlueprintType)
enum class ESurvivorState : uint8
{
	Death UMETA(DisplayName = "Death"),			// 죽음 상태
	Hp1	UMETA(DisplayName = "Hp1"),				// 체력이 1인 상태
	Hp2	UMETA(DisplayName = "Hp2"),				// 체력이 2인 상태
	Hp3	UMETA(DisplayName = "Hp3"),				// 체력이 3인 상태
	Piggyback UMETA(DisplayName = "Piggyback"),	// 업힌 상태
	Hang UMETA(DisplayName = "Hang"),			// 갈고리에 걸린 상태	
};

UCLASS()
class DBD_API ADBD_Player : public ADBDCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pallet", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SearchGimmickSphere;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Run();
	void RunStop();
	void Crouch();
	void CrouchStop();
	void PushInteractGenerator();
	void NonPushInteractGenerator();
	void Parkour();
	void DropdownPallet();
	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 임시용
	void PlusHp();
	void MinusHp();


	void Interaction();
	
	// 플레이어와 근처의 판자를 가져와서 할당하는 함수
	void GetNearPallet();

	// 상태에 따른 플레이어 애니메이션 변경함수
	void ChangePlayerAnimation();


public:
	ADBD_Player();
	// 생존자의 HP 업데이트 함수
	void UpdateHP(int32 Damage);
	void UpdateSpeed();
	void ParkourFinish();


	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline int32 GetHealth() const {return Health;}
	inline bool GetIsRunning() const {return IsRunning;}
	inline bool GetCrouching() const {return IsCrouching;}
	inline bool GetIsInteractGenerator() const {return IsInteractGenerator;}
	inline bool GetIsPiggyBack() const {return IsPiggyback;}
	inline bool GetIsHang() const {return IsHang;}


	// 다른 클래스에서도 사용할 수 있도록
	void ChangeSurvivorState(ESurvivorState survivorState);


	// PlayerStatr Get함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline ESurvivorState GetSurvivorState() const {return SurvivorState;}


private:
	// 임시용 체력 변경 키값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PlusHpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MinusHpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParkourAction;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* StateMontage;

	// 플레이어 상태값
	//UPROPERTY(EditAnywhere, Category = "PlayerState")
	ESurvivorState SurvivorState;

	// 생존자 체력
	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 Health; // 3 : 멀쩡 2 : 부상 1 : 중상 0 : 죽음

	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 MaxHealth = 3; 

	// 상태값 체크 변수 (업힌상태, 갈고리에 걸린 상태)
	bool IsPiggyback = false;
	bool IsHang = false;


	// 뛰는지 안뛰는지 체크 변수
	bool IsRunning = false;
	// Crouch 상태인지 체크 변수
	bool IsCrouching = false;
	// Generator 상호작용 체크 변수 
	bool IsInteractGenerator = false;
	bool IsInteractWindows = false;



	// LineTrace와 Actor들과 닿았는지 체크하는 변수
	bool IsReachGenerator = false;
	bool IsReachWindows = false;

	// 싱호작용 중인 기믹 저장 변수 - GetNearGimmick 함수에서 할당됨
	UPROPERTY(VisibleAnywhere)
	class APallet* NearPallet;

};