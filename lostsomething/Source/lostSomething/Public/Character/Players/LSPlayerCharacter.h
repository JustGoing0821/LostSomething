// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "InputActionValue.h"
#include "LSPlayerCharacter.generated.h"



UCLASS()
class LOSTSOMETHING_API ALSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALSPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	void PossessedBy(AController* NewController) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void Interaction();
	void Attack();

	//Take Damgae
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);







//// Stat Section 캐릭터 스탯 컴포넌트 포인터 선언
//protected:
//	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAcess = "True"))
//	TObjectPtr<class ULSCharacterStatComponent> Stat;
//
//// UI Widjet Section 위젯 컴포넌트 포인터 선언
//protected:
//	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAcess = "True"))
//	TObjectPtr<class UWidgetComponent> HpBar;
//


};
