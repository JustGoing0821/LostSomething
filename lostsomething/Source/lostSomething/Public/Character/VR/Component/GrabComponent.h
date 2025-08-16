// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GrabComponent.generated.h"


class UMotionControllerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDroppedSignature);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOSTSOMETHING_API UGrabComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabComponent();

	// 델리게이트 프로퍼티 선언 (블루프린트에서 바인딩 가능하도록)
	UPROPERTY(BlueprintAssignable, Category = "Grab")
	FOnGrabbedSignature OnGrabbed;

	UPROPERTY(BlueprintAssignable, Category = "Grab")
	FOnDroppedSignature OnDropped;

	bool TryGrab(UMotionControllerComponent* MotionController);

	bool TryRelease();

	void SetShouldSimulateOnDrop();

	void SetPrimitiveCompPhysics(bool bSimulate);

	EControllerHand GetHeldByHand();

	UPROPERTY(EditAnywhere)
	EControllerHand GrabType;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UMotionControllerComponent* MotionControllerRef;
	UGrabComponent* PrimaryGrabComp;
	bool bIsHeld;
	FRotator PrimaryGrabRelativeRotation;
	bool bSimulateOnDrop;



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
