// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"                    
#include "GameFramework/SpringArmComponent.h"        
#include "EnhancedInputComponent.h"                   
#include "EnhancedInputSubsystems.h"                  
#include "InputMappingContext.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Character/Components/LSHpComponent.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "Character/Item/LSItemStructures.h"
#include "Interface/LSCombineTutorialInterface.h"
#include "Character/Players/LSCharacterChoice.h"
#include "LSPlayer.generated.h"

class UInputAction;
class UInputMappingContext;
class UInventoryWidget;


USTRUCT()
struct FCallElevatorParams
{
	GENERATED_BODY()

	UPROPERTY()
	UPrimitiveComponent* HitComponent;

	UPROPERTY()
	AActor* InteractingActor;

	UPROPERTY()
	FVector ObjectTracingTheLine;
};



UCLASS()
class LOSTSOMETHING_API ALSPlayer : public ACharacter, public ILSTakeDamageInterface, public ILSCombineTutorialInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;


public:
	ALSPlayer();

	virtual void PostInitializeComponents() override;


protected:
	void StartGame();

public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCStartGame();



public:
	

	//outline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	UMaterialInterface* ItemOverlayMaterial;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlay")
	UMaterialInterface* ItemOverlayMaterial2;


	void ApplyOverlayMaterialToActor(AActor* TargetActor, UMaterialInterface* OverlayMaterial);
	void RemoveOverlayMaterialToActor(AActor* TargetActor, UMaterialInterface* OverlayMaterial);

	//void RemoveOverlayMaterialFromActor(AActor* TargetActor);


	AActor* PreviousOverlayActor = nullptr;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }


	//Take Damage Section
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	virtual bool isCombining() override;
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION()
	void OnHpChanged(float NewHp);

	// 아이템 픽업 함수_ Pick Item 
	// 인풋 변수가 item details. 
	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void PickItem(const FItemDetails& PickedItemInfo);

	

	//voice
	void VoiceStart(const FInputActionValue& Value);
	void VoiceStop(const FInputActionValue& Value);

	//Item Section

	//줍기 (Destroy)
	
	//줍기 내부
	void PickUpCore();

	UFUNCTION(Server, Reliable)
	virtual void ServerPickUpCore(AMasterItem* TargetItem);

	UFUNCTION(NetMulticast, Reliable)
	void MultiPickUpCore(AActor* TargetItem);

	UFUNCTION(Client, Reliable)
	void ClientPickUpCore(FItemDetails ItemData);
	
	//줍기 애니메이션
	void Drop();
	

	UFUNCTION(Server, Reliable)
	void ServerDrop();

	UFUNCTION(NetMulticast, Reliable)
	void MultiDrop();

	//UFUNCTION(Client, Reliable)
	//void ClientPickUp(FItemDetails ItemData);


	//버리기 (Item Drop : Spawn)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItemFromSlot();

	UFUNCTION(Server, Reliable)
	void ServerDropItemFromSlot(TSubclassOf<AMasterItem> ItemClass, FVector SpawnLocation, FRotator SpawnRotation, int32 SlotIndex);

	UFUNCTION(NetMulticast, Reliable)
	void MultiDropItemFromSlot(TSubclassOf<AMasterItem> ItemClass, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(Client, Reliable)
	void ClientDropItemFromSlot(int32 SlotIndex);


	//던지기 (impulse로 아이템 스폰)
	void SpawnThrowableItem(const FItemDetails& ItemToThrow);
	

	UFUNCTION(Server, Reliable)
	void ServerSpawnThrowableItem(const FItemDetails& ItemToThrow);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnThrowableItem(const FItemDetails& ItemToThrow);

	UFUNCTION(Client, Reliable)
	void ClientSpawnThrowableItem(int32 SlotIndex);

	//슬롯 관련
	//아이템 픽업후 슬롯에 넣기
	//입력 파라미터 itemdetials 구조체
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickItemInSlot(const FItemDetails& PickedItem);

	// 아이템 던지기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void ThrowItem();


	// 인벤토리 초기화 
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory();


	// Drop Item 위치를 나타내는 Arrow 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UArrowComponent> DropItemLoc;



	
	// 슬롯 선택 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SelectedSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxSlots = 4;

	//숫자키로 슬롯선택
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SelectSlot(int32 SlotIndex);

	// 슬롯 선택 시스템 함수들
	/*UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ChangeSlot(int32 NewSlot);*/

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//int32 GetSelectedSlot() const { return SelectedSlot; }

	
	virtual void Attack();

	UFUNCTION(Server, Reliable)
	void ServerAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MultiAttack();





	void ProcessAttack();

	

	UFUNCTION(Server, Reliable)
	void ServerProcessAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MultiProcessAttack();

	UFUNCTION(Client, Reliable)
	void ClientProcessAttack();


protected:
	/*************************************Function**************************************/

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	//void Interreact();
	void Interaction();
	void ApplyDamage(float DamageAmount);

	//Interaction Section
	void PerformLineTrace();
	bool IsActorName(AActor* InActor, const FString& InString) const;
	void CallElevator(AActor* InActor);


	UPROPERTY(Replicated)
	TObjectPtr<AActor> CurrentDectectActor;

	UPROPERTY(Replicated)
	FColor TickDectectResultColor;

	UPROPERTY(Replicated)
	FString AimScript;

	UPROPERTY(Replicated)
	FCallElevatorParams CallElevatorParams;



	//void OnMouseWheelUp(const FInputActionValue& Value);
	//void OnMouseWheelDown(const FInputActionValue& Value);
	void OnSelectSlot1();
	void OnSelectSlot2();
	void OnSelectSlot3();
	void OnSelectSlot4();
	void OnSelectSlot5();

	// 죽음 상태 변수
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Death")
	bool bIsDead = false;

	// 부활 타이머
	FTimerHandle RespawnTimerHandle;
	FTimerHandle MeshHideTimerHandle;

	UFUNCTION()
	void MeshHide();

	// 죽음, 부활 함수
	UFUNCTION()
	void Die();

	UFUNCTION(Server, Reliable)
	void ServerDie();

	UFUNCTION(NetMulticast, Reliable)
	void MultiDie();

	UFUNCTION(Client, Reliable)
	void ClientDie();


	UFUNCTION()
	void Respawn();

	UFUNCTION(Server, Reliable)
	void ServerRespawn();

	UFUNCTION(NetMulticast, Reliable)
	void MultiRespawn();

	UFUNCTION(Client, Reliable)
	void ClientRespawn();


	// HP가 0이 될 때 호출함수
	UFUNCTION()
	void OnHpReachedZero(float ZeroHp);


	/*************************************Property**************************************/


	//camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* VoiceAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<class UInputAction> InterreactAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DropAction;
	//줍기


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	ULSHpComponent* HpComponent;


	UPROPERTY(ReplicatedUsing = OnRep_CurrentHp)
	float CurrentHp;

	UFUNCTION()
	void OnRep_CurrentHp();

	// 마우스 휠 액션들 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MouseWheelUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MouseWheelDownAction;

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


	// 인벤토리 아이템 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FItemDetails> ItemInfoArray;


	//궤적
	 // 초기 속력 (cm/s)
public:
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

	// 미리보기 상태
	bool bThrowPreview = false;
	TArray<FVector> CachedPathPoints;

	// 미리보기 제어
	void StartThrowPreview();

	UFUNCTION(Server, Reliable)
	void ServerStartThrowPreview();

	UFUNCTION(NetMulticast, Reliable)
	void MultiStartThrowPreview();


	void UpdateThrowPreview();


	void EndThrowPreview(bool bDoThrow);


	UFUNCTION(NetMulticast, Reliable)
	void MultiEndThrowPreview(bool bDoThrow);

	UFUNCTION(Server, Reliable)
	void ServerEndThrowPreview(bool bDoThrow);
	






	// 공용 계산기
	FVector ComputeThrowInitialVelocity() const;

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

	// 계산 함수
	float GetThrowAlphaFromPitch() const;                // Pitch -> [0..1]
	FVector ComputeThrowInitialVelocity_ByCamPitch() const;




/********************Renew Combine System************************/
//main
protected:
	void ChangeCombineState(uint8 InIsCombining);
	void ApplyCombineState();
	void HandleCombinedIJaeMovement();

public:
	FORCEINLINE void SetbIsCombining(uint8 InIsCombining) { bIsCombining = InIsCombining; }
	FORCEINLINE ELSCharacterChoice& GetCharacterChoice() { return CharacterChoice; }

protected:
	UPROPERTY(Replicated)
	uint8 bIsCombining : 1;

	UPROPERTY(Replicated)
	ELSCharacterChoice CharacterChoice;

	UPROPERTY(Replicated)
	TObjectPtr<APawn> PusherSiJaeCharacter;

	UPROPERTY(Replicated)
	TObjectPtr<APawn> PushedIJaeCharacter;

	float CombineDistance = 50.0f;


	//Check Distance
protected:
	void CheckCombineDistance();
	bool IsPlayerNear();

protected:
	float MaxCombineDistance = 100.0f;


	//Stemina
protected:
	void DecreaseCombineStamina();
	void IncreaseCombineStamina();
	void UpdateCombineStaminaWidget(float InCurrentStemina);

protected:
	FTimerHandle CombineStaminaTimer;

	float CurrentCombineStamina;

	const float MaxCombineStamina = 100.0f;
	const float CombineStaminaDecreaseRate = 10.0f;
	const float CombineStaminaIncreaseRate = 20.0f;
	const float CombineStaminaIncreaseDelay = 2.0f;


//RenewWheelchair System Test - RPC
public:
	UFUNCTION(Server, Unreliable)
	void ServerRPCChangeCombineState(uint8 InIsCombining);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCApplyCombineState();

	UFUNCTION(Client, Unreliable)
	void ClientRPCUpdateCombineStaminaWidget(float InCurrentStemina);
};
