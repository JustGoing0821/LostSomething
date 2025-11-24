// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Engine/TimerHandle.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Character/Item/LSItemStructures.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Game/LSNetworkPosition.h"
#include "Interface/LSCombineTutorialInterface.h"
#include "Sound/SoundBase.h"
#include "LSPlayer.generated.h"

USTRUCT()
struct FCallElevatorParams
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class UPrimitiveComponent> HitComponent;

	UPROPERTY()
	TObjectPtr<class AActor> InteractingActor;

	UPROPERTY()
	FVector ObjectTracingTheLine;
};

UCLASS()
class LOSTSOMETHING_API ALSPlayer : public ACharacter, public ILSTakeDamageInterface, public ILSCombineTutorialInterface
{
	GENERATED_BODY()

/*******************

기본적인 함수 override 영역
여기 위로 뭐 선언하지 마시오 밑에다가 섹션 추가해서 선언하시오
열심히 정리해놨으니까!!!!!!!!!!!!!!!!!!!!!!

호소문 추가 : 헤더파일은 상속할 인터페이스나 Enum헤더 아니면 include하지 말고 >>>class 전방선언<<<을 쓰시오.
그리고 기왕이면 날것의 포인터 쓰지 말고 언리얼에서 제공하는 >>>포인터 TObjectPtr<<<<을 쓰시오.

예시 : TObjectPtr<class USkeletalMeshComponent>
함수 예시 : void VoiceStart(const class FInputActionValue& Value); 이런식으로 쓰면 됨

헤더 너무 많이 include하지마 최적화 어려워!!!!!!!

호소문 추가 2 : protected로 선언할 수 있으면 최대한 proteced로 선언하시고,
섹션 시작할 때마다 지정자 추가하고 선언 시작하시오.

********************/
	
public:
	ALSPlayer();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

/*******************

Start Game Section

********************/
protected:
	void StartGame();

	//RPC
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCStartGame();



/*******************

Mesh Section

********************/
protected:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FirstPersonCameraComponent;

	//camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE TObjectPtr<class USkeletalMeshComponent> GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE TObjectPtr<class UCameraComponent> GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }



/*******************

 Input Section

********************/
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> VoiceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DropAction;

	//슬롯 선택 키
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SelectSlot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SelectSlot2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SelectSlot3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SelectSlot4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SelectSlot5Action;

	// 마우스 휠 액션들 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MouseWheelUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MouseWheelDownAction;



/*******************

Basic Section

********************/
protected:
	virtual void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);



/*******************

Interaction Section

********************/
protected:
		UPROPERTY(Replicated)
		TObjectPtr<AActor> CurrentDectectActor;

		UPROPERTY(Replicated)
		FColor TickDectectResultColor;

		UPROPERTY(Replicated)
		FString AimScript;

		UPROPERTY(Replicated)
		FCallElevatorParams CallElevatorParams;

protected:
	void PerformLineTrace();
	void Interaction();
	void CallElevator(AActor* InActor);



/*******************

 Overlay Section

********************/
protected:
		//outline
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
		TObjectPtr<class UMaterialInterface> ItemOverlayMaterial;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
		TObjectPtr<class UMaterialInterface> ItemOverlayMaterial2;

		TObjectPtr<AActor> PreviousOverlayActor = nullptr;

protected:
	void ApplyOverlayMaterialToActor(AActor* TargetActor, UMaterialInterface* OverlayMaterial);
	void RemoveOverlayMaterialToActor(AActor* TargetActor, UMaterialInterface* OverlayMaterial);



/*******************

 Damage Section

********************/
public:
	// 죽음 상태 변수
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Death")
	bool bIsDead = false;

protected:
		

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class ULSHpComponent> HpComponent;

		UPROPERTY(ReplicatedUsing = OnRep_CurrentHp)
		float CurrentHp;

		// 부활 타이머
		FTimerHandle RespawnTimerHandle;
		FTimerHandle MeshHideTimerHandle;


public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	void ApplyDamage(float DamageAmount);

	UFUNCTION()
	void OnHpChanged(float NewHp);

	UFUNCTION()
	void OnRep_CurrentHp();

	UFUNCTION()
	void MeshHide();

	// 죽음, 부활 함수
	UFUNCTION()
	void Die();

	UFUNCTION()
	void Respawn();

	

	// HP가 0이 될 때 호출함수
	UFUNCTION()
	FORCEINLINE void OnHpReachedZero(float ZeroHp) { Die(); }

	//RPC
public:
	UFUNCTION(NetMulticast, Reliable)
	void MultiDie();

	UFUNCTION(Server, Reliable)
	void ServerDie();

	UFUNCTION(NetMulticast, Reliable)
	void MultiRespawn();



/*******************

Voice Section

********************/
protected:
	void VoiceStart(const FInputActionValue& Value);
	void VoiceStop(const FInputActionValue& Value);



/*******************

Item	Section

********************/
protected:
		ELSNetworkPosition GetNetworkPositionForInventory() const;

		

		// Drop Item 위치를 나타내는 Arrow 컴포넌트
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UArrowComponent> DropItemLoc;

		// 슬롯 선택 변수들
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 SelectedSlot = 0;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 MaxSlots = 4;

		// 인벤토리 아이템 배열
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<FItemDetails> ItemInfoArray;

public:
	//줍기 내부
	void PickUpCore();

protected:
	//줍기 애니메이션
	void Drop();

	//버리기 (Item Drop : Spawn)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItemFromSlot();

	//슬롯 관련
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickItemInSlot(const FItemDetails& PickedItem);

	// 인벤토리 초기화 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory();

	//inventory instance
	void SaveInventoryToGameInstance();
	void LoadInventoryFromGameInstance();

	//숫자키로 슬롯선택
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SelectSlot(int32 SlotIndex);

	void OnSelectSlot1();
	void OnSelectSlot2();
	void OnSelectSlot3();
	void OnSelectSlot4();
	void OnSelectSlot5();

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void PickItem(const FItemDetails& PickedItemInfo);

	void OnMouseWheelUp(const FInputActionValue& Value);
	void OnMouseWheelDown(const FInputActionValue& Value);
	void ChangeSlot(int32 Offset);


	// 슬롯 선택 시스템 함수들
	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void ChangeSlot(int32 NewSlot);

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//int32 GetSelectedSlot() const { return SelectedSlot; }

	//RPC
public:
	UFUNCTION(Server, Reliable)
	virtual void ServerPickUpCore(AMasterItem* TargetItem);

	UFUNCTION(NetMulticast, Reliable)
	void MultiPickUpCore(AActor* TargetItem);

	UFUNCTION(Client, Reliable)
	void ClientPickUpCore(FItemDetails ItemData);
	
	UFUNCTION(Server, Reliable)
	void ServerDrop();

	UFUNCTION(NetMulticast, Reliable)
	void MultiDrop();

	UFUNCTION(Server, Reliable)
	void ServerDropItemFromSlot(TSubclassOf<AMasterItem> ItemClass, FVector SpawnLocation, FRotator SpawnRotation, int32 SlotIndex);

	UFUNCTION(Client, Reliable)
	void ClientDropItemFromSlot(int32 SlotIndex);

	//UFUNCTION(Client, Reliable)
	//void ClientPickUp(FItemDetails ItemData);


/*******************

 Weapon Section

********************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FName WeaponSocketName = TEXT("WeaponSocket"); 

	//액터 말고 컴포너늩로
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* EquippedWeaponMesh;

	// 손에 무기 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AMasterItem* EquippedWeapon = nullptr;

	// 장착,해제
	void RefreshWeaponEquipFromCurrentSlot();

	

	void ApplyWeaponVisualFromItem(const FItemDetails& Info);

	UFUNCTION(Server, Reliable)
	void ServerApplyWeaponVisualFromItem(const FItemDetails& Info);

	UFUNCTION(NetMulticast, Reliable)
	void MultiApplyWeaponVisualFromItem(const FItemDetails& Info);

/*******************

 Throw Section

********************/
protected:
	// 미리보기 상태
	bool bThrowPreview = false;
	TArray<FVector> CachedPathPoints;

	//궤적
	// 초기 속력 (cm/s)
	UPROPERTY(EditAnywhere, Category = "Throw|Preview")
	float ThrowSpeed = 1200.f;

	// 전방 + 위로 올리는 비율
	UPROPERTY(EditAnywhere, Category = "Throw|Preview")
	float ThrowUpRatio = 0.3f;

	// 예측 시간 (초)
	UPROPERTY(EditAnywhere, Category = "Throw|Preview")
	float PreviewTime = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Throw|Preview")
	int32 PreviewSegments = 20;         // 궤적 샘플 개수

	UPROPERTY(EditAnywhere, Category = "Throw|Preview")
	float ProjectileRadius = 5.f;       // 충돌 반경

	// === Throw by Camera Pitch ===
	UPROPERTY(EditAnywhere, Category = "Throw|CamPitch")
	float PitchMinDeg = -20.f;   // 이 각도일 때 가장 가깝게 (고개 약간 숙임)

	UPROPERTY(EditAnywhere, Category = "Throw|CamPitch")
	float PitchMaxDeg = 40.f;    // 이 각도일 때 가장 멀리 (고개 위로 듦)

	UPROPERTY(EditAnywhere, Category = "Throw|CamPitch")
	float SpeedNear = 800.f;     // 가까운 투척 속도 (cm/s)

	UPROPERTY(EditAnywhere, Category = "Throw|CamPitch")
	float SpeedFar = 2200.f;    // 먼 투척 속도 (cm/s)

	UPROPERTY(EditAnywhere, Category = "Throw|CamPitch")
	float UpBiasNear = 0.10f;    // 가까울 때 위쪽 가중치

	UPROPERTY(EditAnywhere, Category = "Throw|CamPitch")
	float UpBiasFar = 0.55f;    // 멀 때 위쪽 가중치

public:
	// 미리보기 제어
	void StartThrowPreview();
	void UpdateThrowPreview();
	void EndThrowPreview(bool bDoThrow);

protected:
	// 아이템 던지기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void ThrowItem();

	//던지기 (impulse로 아이템 스폰)
	void SpawnThrowableItem(const FItemDetails& ItemToThrow);

	// 공용 계산기
	FVector ComputeThrowInitialVelocity() const;
	// 계산 함수
	float GetThrowAlphaFromPitch() const;                // Pitch -> [0..1]
	FVector ComputeThrowInitialVelocity_ByCamPitch() const;
	
	//RPC
public:
	UFUNCTION(Server, Reliable)
	void ServerSpawnThrowableItem(const FItemDetails& ItemToThrow);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnThrowableItem(const FItemDetails& ItemToThrow);

	UFUNCTION(Server, Reliable)
	void ServerStartThrowPreview();

	UFUNCTION(NetMulticast, Reliable)
	void MultiStartThrowPreview();

	UFUNCTION(Server, Reliable)
	void ServerEndThrowPreview(bool bDoThrow);

	UFUNCTION(NetMulticast, Reliable)
	void MultiEndThrowPreview(bool bDoThrow);



/*******************

 Attack Section

********************/
public:
	virtual void Attack();
	void ProcessAttack();

	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;

	//RPC
public:
	UFUNCTION(Server, Reliable)
	void ServerAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MultiAttack();

	UFUNCTION(Server, Reliable)
	void ServerProcessAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MultiProcessAttack();



/*******************

Combine Section

********************/
public:
	virtual bool isCombining() override;

protected:
	//Main
	UPROPERTY(Replicated)
	uint8 bIsCombining : 1;

	UPROPERTY(Replicated)
	ELSCharacterChoice CharacterChoice;

	UPROPERTY(Replicated)
	ALSPlayer* PusherSiJaeCharacter;

	UPROPERTY(Replicated)
	ALSPlayer* PushedIJaeCharacter;

	const float CombineDistance = 80.0f;

	//Check Distance
	const float MaxCombineDistance = 150.0f;

	//Stemina
	FTimerHandle CombineStaminaTimer;
	float CurrentCombineStamina;
	const float MaxCombineStamina = 100.0f;
	const float CombineStaminaDecreaseRate = 10.0f;
	const float CombineStaminaIncreaseRate = 20.0f;
	const float CombineStaminaIncreaseDelay = 2.0f;

public:
		FORCEINLINE void SetbIsCombining(uint8 InIsCombining) { bIsCombining = InIsCombining; }
		FORCEINLINE ELSCharacterChoice& GetCharacterChoice() { return CharacterChoice; }

protected:
	//Main
	void ChangeCombineState(uint8 InIsCombining);
	void ApplyCombineState();
	void HandleCombinedIJaeMovement();

	//Check Distance
	void CheckCombineDistance();
	bool IsPlayerNear();

	//Stemina
	void DecreaseCombineStamina();
	void IncreaseCombineStamina();
	void UpdateCombineStaminaWidget(float InCurrentStemina);

	//RPC
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCChangeCombineState(uint8 InIsCombining);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCApplyCombineState();

	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateCombineStaminaWidget(float InCurrentStemina);



/*******************

 ETC Section

********************/
protected:
	bool IsActorName(AActor* InActor, const FString& InString) const;




};
