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
#include "Interface/LSCharacterWidgetInterface.h"
#include "InputAction.h"
#include "Character/Stat/LSCharacterStatComponent.h"
#include "Interface/LSWheelchairInterface.h"
#include "LSPlayer.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSPlayer : public ACharacter, public ILSTakeDamageInterface, public ILSCharacterWidgetInterface, public ILSWheelchairInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALSPlayer();

	virtual void PostInitializeComponents() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Interaction();
	void Attack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Take Damage Section
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }




// Stat
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULSCharacterStatComponent> Stat;

// UI Widget
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULSWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class ULSUserWidget* InUserWidget) override;

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
