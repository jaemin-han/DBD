// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DBD_Interface_Gimmick.h"
#include "Generator.generated.h"

DECLARE_DELEGATE(FOnGenerateFail);


// 발전기 액터 클래스 -> 생존자의 발전기 애니메이션 상호작용
UCLASS()
class DBD_API AGenerator : public AActor, public IDBD_Interface_Gimmick
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interaction(APawn* Caller = nullptr) override;
	virtual void FailedInteraction() override;
	FORCEINLINE virtual FString GetGimmickName() override;
	FORCEINLINE virtual FString GetInteractKey() override;

	inline bool SetIsActivatedSkillCheckZone(bool value) { return IsActivatedSkillCheckZone = value; }

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetGaugeUIVisibility(ESlateVisibility visibility);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_FailedInteract();

	UStaticMeshComponent* GetMeshComp() const { return GeneratorMeshComp; }

public:
	// 발전기 게이지 UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UGaugeUI> GaugeUIWidgetClass;

	UPROPERTY()
	class UGaugeUI* GaugeUI;

	//UPROPERTY()
	FOnGenerateFail OnGenerateFail;
	
	UPROPERTY(EditAnywhere)
	class UAudioComponent* GeneratorSoundComp;
	float GeneratorSoundTimer = 0.0f;
	// 발전기 활성화가 됬는지 판단 여부 변수
	UPROPERTY(EditAnywhere, Category = "Generator")
	bool IsActivated = false;


private:
	/// 발전기의 이름과 상호작용 키
	FString Name = TEXT("Generator");
	FString InteractKey = TEXT("LBM");

	float Percent = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float RepairMultiplier = 1.0f;
	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsFullGauge = false;
	
	bool GetIsFullGauge() const { return IsFullGauge; }
	void UpdateGauge(float time);

	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SetGaugeUIPercent(float value);







/// RoundGaugeUI관련된 변수 및 함수

	bool IsActivatedSkillCheckZone = false;			/// 스킬체크 존 활성화 여부
	bool IsCheckRoundGauge = false;					/// 라운드 게이지 활성화 여부
	float RoundPercent = 0.0f;						/// 라운드 게이지 퍼센트

	float SkillCheckZoneStart = 0.0f;				/// 스킬체크 존 시작 지점
	float SkillCheckZoneEnd = 0.0f;					/// 스킬체크 존 끝 지점

	bool IsSuccessSkillCheck = false;				/// 스킬체크 성공 여부

public:
	void CheckRoundGauge(float frame);										// 라운드 게이지 활성화 여부 판단 함수
	UFUNCTION(Client, Reliable)
	void Client_VisibleRoundGauge(float time, bool IsVisible);							// 라운드 게이지 활성화 여부 판단 함수 클라이언트 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetRoundGaugePercent(bool IsVisible, float per);				// 라운드 게이지 퍼센트 설정 함수


	void SetSkillCheckZone(float value);									// 스킬체크 존 설정 함수 (서버에서 설정 -> Multicast로 클라이언트들에게 전달)

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetSkillCheckZone(float ran, FWidgetTransform newTrans);		// 스킬체크 존 설정 함수 (클라이언트에서 설정 -> Multicast로 서버에게 전달)


	bool IsSuccessedSkillCheck();											// 스킱체크 활성화 됬을때 스킬체크가 성공했는지 실패했는지 판단 함수
};
