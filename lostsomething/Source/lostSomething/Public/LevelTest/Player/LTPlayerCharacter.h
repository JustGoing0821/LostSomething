// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "LevelTest/Interface/LTTakeDamageInterface.h"
#include "Interface/LSCombineTutorialInterface.h"
#include "LTPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerCharacter : public ATP_ThirdPersonCharacter, public ILTTakeDamageInterface, public ILSCombineTutorialInterface
{
	GENERATED_BODY()
	
public:
	ALTPlayerCharacter();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	void Interaction();

//Take Damage Section
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

//WheelChairTutorial
public:
	virtual bool isCombining() override;
};
