// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "DBD_Player.generated.h"


UENUM(BlueprintType)
enum class ESurvivorState : uint8
{
	Death		UMETA(DisplayName = "Death"),			// 죽음 상태
	Hp1			UMETA(DisplayName = "Hp1"),				// 체력이 1인 상태
	Hp2			UMETA(DisplayName = "Hp2"),				// 체력이 2인 상태
	Hp3			UMETA(DisplayName = "Hp3"),				// 체력이 3인 상태
	Piggyback	UMETA(DisplayName = "Piggyback"),		// 업힌 상태
	Hang		UMETA(DisplayName = "Hang"),			// 갈고리에 걸린 상태	
};

UCLASS()
class DBD_API ADBD_Player : public ADBDCharacter
{
	GENERATED_BODY()

	ADBD_Player();

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
	void ExitDoor();
	void GeneratorSkillCheck();
	void ReleasedGeneratorSkillCheck();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 임시용
	void PlusHp();
	void MinusHp();

	// 플레이어와 기믹이 충돌할때 발생하는 함수
	void Interaction();

	// 플레이어와 액터가 충돌할때 발생하는 함수 -> 액터와만 오버랩 반응을 하게됨.
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
	// 플레이어와 근처의 판자를 가져와서 할당하는 함수
	void GetNearPallet();

	// 상태에 따른 플레이어 애니메이션 변경함수
	void ChangePlayerAnimation();

	void SpawnDecal();

	// 베지에 곡선을 활용한 파쿠르 애니메이션 만들기
	FVector CalculateQuadraticBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2);
	FVector CalculateBezierPoint(float t, const FVector& p0, const FVector& p1, const FVector& p2, const FVector& p3);
	void MoveAlongQuadraticBezier(float DeltaTime);

	FVector P0;
	FVector P1;
	FVector P2;
	FVector P3;


public:
	
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
	inline bool GetIsSpaceBar() const {return IsSpaceBar;}

	// 다른 클래스에서도 사용할 수 있도록
	void ChangeSurvivorState(ESurvivorState survivorState);

	void VisibleMainUI(bool IsVisible, FString Name, FString Key);


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
	UInputAction* GeneratorAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ParkourAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ExitAction;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* StateMontage;

	// 플레이어 상태값
	//UPROPERTY(EditAnywhere, Category = "PlayerState")
	ESurvivorState SurvivorState;


	// MainUI 추가
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInteractionUI> MainUIClass;		// 이후 플레이 UI가 추가된다면 MainUI 클래스로 변경
	UPROPERTY()
	class UInteractionUI* MainUI;


	class IDBD_Interface_Gimmick* Gimmick;
	

	// DecalComponent 추가하기
	UPROPERTY(EditAnywhere, Category = "Decal")
	TSubclassOf<class ADecal> DecalFactory;

	float SpawnDecalTime = 0.2f;

	// 생존자 체력
	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 Health; // 3 : 멀쩡 2 : 부상 1 : 중상 0 : 죽음

	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 MaxHealth = 3; 

	// 상태값 체크 변수 (업힌상태, 갈고리에 걸린 상태)
	bool IsPiggyback = false;
	bool IsHang = false;


	// 뛰는지 안뛰는지 체크 변수
	bool IsRunning = false;				// 뛰는중이니?
	// Crouch 상태인지 체크 변수
	bool IsCrouching = false;			// 앉아있니?
	// 파쿠르 중인지 체크 변수
	bool IsParkour = false;				// 파쿠르 중이니?

	// 스페이스바를 눌렀는지
	bool IsSpaceBar = false;			// 스페이스바 눌렀니?


	// Generator 상호작용 체크 변수 
	bool IsInteractGenerator = false;	// 발전기와 상호작용 했니? -> 했으면 : 발전기 애니메이션 실행, 게이지 시작
	bool IsInteractWindows = false;		// 창문과 상호작용 했니? -> 했으면 : 파쿠르 애니메이션 실행
	bool IsInteractDoor = false;		// 문과 상호작용 했니? -> 했으면 : 문 게이지 애니메이션 실행, 게이지 시작
	bool IsSkillCheckZone = false;		// 스킬체크가 활성화 됬나?
	bool IsInteractPallet = false;		// 판자와 상호작용 했니? -> 했으면 : 판자 애니메이션 실행


	// LineTrace와 Actor들과 닿았는지 체크하는 변수
	bool IsReachGenerator = false;		// 발전기와 라인트레이스가 닿았니?
	bool IsReachWindows = false;		// 창문이랑 라인트레이스가 닿았니?

	// 출입구와 오버랩됬는지 확인 bool 변수
	bool IsOverlapDoor = false;			// 문과 오버랩 됬니?

	// 싱호작용 중인 기믹 저장 변수 - GetNearGimmick 함수에서 할당됨
	UPROPERTY(VisibleAnywhere)
	class APallet* NearPallet;

	class ADoor* Door;	// 출입구 액터 저장 변수

	class AWindows* Window;
};