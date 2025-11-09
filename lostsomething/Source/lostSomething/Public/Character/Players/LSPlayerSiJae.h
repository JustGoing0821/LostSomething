// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayer.h"
#include "Character/Players/LSPlayerIJae.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSDarkWidget.h"
#include "LSPlayerSiJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerSiJae : public ALSPlayer
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	//virtual void GetDistance()

	UPROPERTY()
	TObjectPtr<ALSPlayerIJae> IJae;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULSDarkWidget> DarkWidgetClass;

	UPROPERTY()
	ULSDarkWidget* DarkWidgetInstance;
	
public: 
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//UStaticMeshComponent* Weapon;

	//void WeaponPickUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wheelchair Settings")
	float WheelchairTurnRate = 90.0f;

	ALSPlayerSiJae();

protected:
	//virtual void Attack() override;
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Jump() override;
	void ThrowItem() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void HandlePusherWheelchairInput(const FVector2D& MovementVector);
	void Tick(float DeltaTime);
};