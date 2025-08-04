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
#include "Interface/LSWheelchairInterface.h"
#include "Interface/LSCombineTutorialInterface.h"

#include "LSPlayer.generated.h"
class UInputAction;
class UInputMappingContext;
class UInventoryWidget;

/*************************************Function**************************************/
/*************************************Property**************************************/


UCLASS()
class LOSTSOMETHING_API ALSPlayer : public ACharacter, public ILSTakeDamageInterface, public ILSWheelchairInterface, public ILSCombineTutorialInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;


public:
	/*************************************Function**************************************/


	ALSPlayer();

	virtual void PostInitializeComponents() override;

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
	
	void PickUp();
	
	
	UFUNCTION(Server, Reliable)
	void ServerPickUp(AMasterItem* TargetItem);

	UFUNCTION(NetMulticast, Reliable)
	void MultiPickUp(AActor* TargetItem);

	UFUNCTION(Client, Reliable)
	void ClientPickUp(FItemDetails ItemData);


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
	void ThrowItem();


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
	void Interreact();
	void Interaction();
	void ApplyDamage(float DamageAmount);

	







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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InterreactAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PickUpAction;
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

	// 휠체어 관련 프로퍼티
	UPROPERTY(EditAnywhere, Category = "Wheelchair")
	float NormalCombineDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Wheelchair")
	float MaxCombineDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Wheelchair")
	float DistanceCheckInterval = 0.1f;

	float LastDistanceCheckTime = 0.0f;


//	//이렇게 쓰지 말기
//	/*ULSInventoryWidget* InventoryWidget;
//	ULSInventoryEntry* InventoryEntryWidget;
//*/



// Wheelchair
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wheelchair")
	bool bIsBeingPushed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wheelchair")
	TObjectPtr<ACharacter> PusherCharacter;

	UPROPERTY(Replicated)
	TObjectPtr<ALSPlayer> PushedWheelchairCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wheelchair")
	bool bCanPushWheelchair;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestWheelchairInteraction(AActor* TargetActor);

	virtual void StartPushingWheelchair_Implementation(ACharacter* Pusher) override;
	virtual void StopPushingWheelchair_Implementation(ACharacter* Pusher) override;
	virtual bool IsBeingPushed_Implementation() const override;
	virtual bool CanPushWheelchair() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartPushingWheelchair(ACharacter* Pusher);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopPushingWheelchair();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastWheelchairStateChanged(bool bPushing, ACharacter* Pusher);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestAutoSeparation();

	void HandleWheelchairMovement();

	void CheckCombineDistance();
	void AutoSeparateFromWheelchair();

public:
	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
