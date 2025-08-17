// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "VRPlayer.generated.h"

class UCameraComponent;
class UMotionControllerComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class UMotionControllerComponent;
class USkeletalMeshComponent;

UCLASS()
class LOSTSOMETHING_API AVRPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// VR Origin & Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	USceneComponent* VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR")
	UCameraComponent* VRCamera;

	// --------------------
	// Left Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR|LeftHand")
	UMotionControllerComponent* LeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR|LeftHand")
	USkeletalMeshComponent* HandLeft;

	// --------------------
	// Right Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR|RightHand")
	UMotionControllerComponent* RightController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR|RightHand")
	USkeletalMeshComponent* HandRight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Enhanced Input Actions */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* VRMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Grab_Left;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Grab_Right;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Toggle_Left;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Toggle_Right;

	/** Input Callbacks */
	void Move(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void Grab_Left(const FInputActionValue& Value);
	void Grab_Right(const FInputActionValue& Value);
	void Toggle_Left(const FInputActionValue& Value);
	void Toggle_Right(const FInputActionValue& Value);

	//UGrabComponent* GetGrabComponentNearMotionController(UMotionControllerComponent MotionController);


};
