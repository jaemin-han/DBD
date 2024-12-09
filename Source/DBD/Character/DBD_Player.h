// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DBDCharacter.h"
#include "DBD_Player.generated.h"

DECLARE_DELEGATE_OneParam(FOnSetGimmickAtRescueDelegate, TScriptInterface<class IDBD_Interface_Gimmick>);

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

/** 기본 상속 함수*/
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 서버 등록 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetCameraInLobby();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetCameraInPlay(class ACameraActor* cam);

	virtual void PossessedBy(AController* NewController) override;

/** 기본 상속 함수*/


/** 임시 함수*/
	void PlusHp();
	void MinusHp();
/** 임시 함수*/


/** Input 함수 */
protected:
	// 움직임 Input 함수
	void Run();
	UFUNCTION(Server, Reliable)
	void ServerRPC_Run();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Run();

	void RunStop();
	UFUNCTION(Server, Reliable)
	void ServerRPC_RunStop();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_RunStop();

	void Crouch();
	UFUNCTION(Server, Reliable)
	void ServerRPC_Crouch();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Crouch();
	
	void CrouchStop();
	UFUNCTION(Server, Reliable)
	void ServerRPC_CrouchStop();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_CrouchStop();

	// 상호작용 Input 함수
	void PushInteractGenerator();
	UFUNCTION(Server, Reliable)
	void ServerRPC_PushInteractGenerator();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_PushInteractGenerator();
	
	void NonPushInteractGenerator();
	UFUNCTION(Server, Reliable)
	void ServerRPC_NonPushInteractGenerator();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_NonPushInteractGenerator();
	
	void DropdownPallet();


	UFUNCTION(Server, Reliable)
	void Server_ExitDoor();
	UFUNCTION(Server, Reliable)
	void Server_NonExitDoor();

	UFUNCTION(Server, Reliable)
	void Server_GeneratorSkillCheck();
	UFUNCTION(Server, Reliable)
	void Server_ReleasedGeneratorSkillCheck();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_GeneratorSkillCheck();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReleasedGeneratorSkillCheck();


	UFUNCTION(Server, Reliable)
	void ServerRPC_Rescue();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Rescue();
/** Input 함수 */


/** 상호작용 함수 */
protected:
	void Interaction();														// 플레이어와 기믹이 충돌할때 발생하는 함수
	void GetNearPallet();													// 플레이어와 판자가 주변에 있는지 탐색하는 함수
	void GetNearSurvivor();													// 플레이어와 다른생존자가 주변에 있는지 탐색하는 함수
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;		// 플레이어와 액터가 충돌할때 발생하는 함수들
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;		// -> 액터와만 오버랩 반응을 하게됨.

	void RaiseFallenSurvivor(ADBD_Player* otherSuvivor);					// 플레이어가 넘어진 생존자를 일으켜세우는 함수
	UFUNCTION(Server, Reliable)
	void Server_RaiseFallenSurvivor();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_RaiseFallenSurvivor(ADBD_Player* otherSuvivor);

	void NonRaiseFallenSurvivor();



	// GaugeUI 활성화작용 함수
	void ActivatedGaugeUI(float time);
	UFUNCTION(Server, Reliable)
	void Server_ActivatedGaugeUI();
	UFUNCTION(Client, Reliable)
	void Client_ActivatedGaugeUI();

/** 상호작용 함수 */



/** 생존자 설정 함수*/
protected:
	void ChangePlayerAnimation();											// 플레이어 애니메이션 변경 함수
public:
	void UpdateHP(int32 Damage);											// 플레이어 체력 업데이트 함수
	// Server RPC for UpdateHP
	UFUNCTION(Server, Reliable)
	void ServerRPC_UpdateHP(int32 Damage);
	// Multicast RPC for UpdateHP
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_UpdateHP(int32 Value);
	
	void UpdateSpeed();														// 플레이어 속도 업데이트 함수
	void ChangeSurvivorState(ESurvivorState survivorState);					// 플레이어 상태 변경 함수
	void VisibleMainUI(bool IsVisible, FString Name, FString Key);			// 플레이어 상호작용 UI를 보이게 하는 함수(현재는 상호작용UI로)

	void VisibleInteractUI();
	UFUNCTION(Server, Reliable)
	void Server_VisibleInteractUI();
	UFUNCTION(Client, Reliable)
	void Client_VisibleInteractUI(bool IsGenerator, bool IsParkour, bool isDoorOverlap);

	void HiddenInteractUI();
	UFUNCTION(Server, Reliable)
	void Server_HiddenInteractUI();
	UFUNCTION(Client, Reliable)
	void Client_HiddenInteractUI();
	
	void SpawnDecal();														// 플레이어가 뛰었을때 발생하는 데칼 생성 함수
/** 생존자 설정 함수*/


	void PrintDebug();



/** Get 함수 */
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline int32 GetHealth() const {return Health;}
	inline bool GetIsRunning() const {return IsRunning;}
	inline bool GetCrouching() const {return IsCrouching;}
	inline bool GetIsInteractGenerator() const {return IsInteractGenerator;}
	inline bool GetIsPiggyBack() const {return IsPiggyback;}
	inline bool GetIsHang() const {return IsHang;}
	inline bool GetIsSpaceBar() const {return IsSpaceBar;}
	// todo: 재민추가 // 희생 시간 Get함수
	float GetSacrificeTime() const {return SacrificeTime;}
	// todo: 재민추가 // 희생 시간 Set함수
	void SetSacrificeTime(const float Time) {SacrificeTime = Time;}
	UPROPERTY(EditDefaultsOnly, Category = "Hanger")
	float TotalSacrificeTime = 120.0f;										// 희생 시간 최대값

	// PlayerStatr Get함수
	UFUNCTION(BlueprintCallable, BlueprintPure)
	inline ESurvivorState GetSurvivorState() const {return SurvivorState;}
/** Get 함수 */


private:
/** 임시 변수*/
	// 임시용 체력 변경 키값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PlusHpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MinusHpAction;
/** 임시 변수*/


/** Input 변수 */
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RaiseUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RescueAction;
/** Input 변수 */

/** 생존자 설정 변수*/
	//UPROPERTY(EditAnywhere, Category = "PlayerState")
	ESurvivorState SurvivorState;							// 플레이어 상태값

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pallet", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SearchGimmickSphere;			// 기믹을 찾기위한 구체 컴포넌트 (상위 클래스인 Character에서 선언할 예정)


	/*MainUI 추가*/
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInteractionUI> MainUIClass;			// 상호작용 UI를 가지고 있을 클래스(이후 플레이 UI가 추가된다면 MainUI 클래스로 변경)
	UPROPERTY()
	class UInteractionUI* MainUI;							// 상호작용 UI

	class IDBD_Interface_Gimmick* Gimmick;					// 상호작용할 기믹 (상위 클래스인 Character에서 선언할 예정)
	/*MainUI 추가*/


	UPROPERTY(EditAnywhere, Category = "Decal")
	TSubclassOf<class ADecal> DecalFactory;					// DecalActor을 담을 클래스
	float SpawnDecalTime = 0.2f;							// DecalActor을 생성할 시간

	// 생존자 체력
	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 Health;											// 체력 변수 3 : 멀쩡 2 : 부상 1 : 중상 0 : 죽음 (이후 없앨수도 있음)

	UPROPERTY(EditAnywhere, Category = "Survivor")
	int32 MaxHealth = 3;									// 최대 체력	(이후 없앨수도 있음)

	/*상태값 체크 변수 (업힌상태, 갈고리에 걸린 상태, 달리는 상태, 앉은 상태)*/
	bool IsPiggyback = false;								// 생존자가 업혀있는 상태인가?
	bool IsHang = false;									// 생존자가 갈고리에 걸린 상태인가?
	bool IsRunning = false;									// 뛰는중이니?
	bool IsCrouching = false;								// 앉아있니?			


	/*Generator 상호작용 체크 변수 */
	bool IsInteractGenerator = false;						// 발전기와 상호작용 했니? -> 했으면 : 발전기 애니메이션 실행, 게이지 시작
	bool IsInteractWindows = false;							// 창문과 상호작용 했니? -> 했으면 : 파쿠르 애니메이션 실행
	bool IsInteractDoor = false;							// 문과 상호작용 했니? -> 했으면 : 문 게이지 애니메이션 실행, 게이지 시작
	bool IsSkillCheckZone = false;							// 스킬체크가 활성화 됬나?
	bool IsInteractPallet = false;							// 판자와 상호작용 했니? -> 했으면 : 판자 애니메이션 실행
	bool IsSpaceBar = false;								// 스페이스바 눌렀니?	(발전기 Round게이지와 관련)

	// 살리는 게이지 타이머
	bool IsClickedRaiseSurvivorButton = false;				// 생존자를 일으키는 버튼을 눌렀니?
	bool IsFinishRaiseSurvivor = false;						// 생존자를 일으키는 게이지가 다 찼니?
	float RaiseSurvivorTimer = 0.0f;						// 생존자를 일으키는 게이지 타이머

	// LineTrace와 Actor들과 닿았는지 체크하는 변수
	UPROPERTY(Replicated)
	bool IsFindGenerator = false;							// 발전기와 라인트레이스가 닿았니?

	// 출입구와 오버랩됬는지 확인 bool 변수
	bool IsOverlapDoor = false;								// 문과 오버랩 됬니?
	bool IsPressExitDoor = false;							// 탈출문을 눌렀니?

	// 싱호작용 중인 기믹 저장 변수 - GetNearGimmick 함수에서 할당됨
	UPROPERTY(VisibleAnywhere, Replicated)
	class APallet* NearPallet;								// 가까이 있는 판자 저장 변수
	class APallet* TracePallet;								// 라인트레이스로 찾은 판자 저장 변수
	UPROPERTY(Replicated)
	ADBD_Player* OtherSurvivor;								// 가까이 있는 다른 생존자 저장 변수
	UPROPERTY(Replicated)
	class ADoor* Door;										// 출입구 액터 저장 변수
	class AWindows* Window;									// 창문 액터 저장 변수

	// todo: 재민추가 // 생존자 남은 희생 시간, 기본 2분 (120초)
	UPROPERTY(EditAnywhere, Replicated, Category = "Hanger")
	float SacrificeTime = TotalSacrificeTime;

	// todo: 재민추가 // Rescue Montage
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* RescueMontage;

	UPROPERTY()
	class ADBDGameState* GameState;

	ACameraActor* SurvivorLobbyCam;


public:
	// rescue 실행 시 anim instance 에 neargimmick 을 set 하는 delegate
	FOnSetGimmickAtRescueDelegate OnSetGimmickAtRescue;

	// sound 관련 변수
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* FootStepSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundAttenuation* FootStepAttenuation;
};