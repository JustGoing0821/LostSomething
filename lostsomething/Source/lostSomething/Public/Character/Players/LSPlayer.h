// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"                    
#include "Character/Item/LSItemDatabase.h"
#include "Character/Item/LSItemBase.h"
#include "Character/Item/Item.h"
#include "GameFramework/SpringArmComponent.h"        
#include "EnhancedInputComponent.h"                   
#include "EnhancedInputSubsystems.h"                  
#include "InputMappingContext.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Character/Components/LSHpComponent.h"
#include "Character/UI/LSInventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
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

public:
	/*************************************Function**************************************/


	ALSPlayer();

	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Take Damage Section
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual bool isCombining() override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//수정
	UFUNCTION()
	void OnHpChanged(float NewHp);

	void DropItem(FItemData ItemData);

	/*************************************Property**************************************/

	TArray<FItemData> Inventory;

	UPROPERTY(EditDefaultsOnly)
	ULSItemDataBase* ItemDatabase;

protected:
	/*************************************Function**************************************/

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Pickup();
	void PickupCheck();
	void Interaction();
	void Attack();
	void FireProjectile();

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	
	//줍기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PickupAction;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInventoryAction> InventoryAction;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	ULSHpComponent* HpComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FireProjectileAction;

	UPROPERTY()
	FHitResult PickupHitResult;
	FVector ViewVector;
	FRotator ViewRotation;


	//투사체 클래스 참조?
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ALSProjectile> ProjectileClass;

	//item
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InventoryEntryWidgetClass;

	//이렇게 쓰지 말기
	/*ULSInventoryWidget* InventoryWidget;
	ULSInventoryEntry* InventoryEntryWidget;
*/

	UPROPERTY()
	TObjectPtr<class ULSInventoryWidget> InventoryWidget;

	UPROPERTY()
	TObjectPtr<class ULSInventoryEntry> InventoryEntryWidget;


//// Stat
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<class ULSCharacterStatComponent> Stat;

// UI Widget
//protected:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
//	TObjectPtr<class ULSWidgetComponent> HpBar;
//
//	virtual void SetupCharacterWidget(class ULSUserWidget* InUserWidget) override;
//





// Wheelchair
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wheelchair")
	bool bIsBeingPushed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wheelchair")
	TObjectPtr<ACharacter> PusherCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wheelchair")
	TObjectPtr<USkeletalMeshComponent> WheelchairMesh;

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

	void HandleWheelchairMovement();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
