// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "LSPlayerCharacter.generated.h"

// 델리게이트 선언

UCLASS()
class LOSTSOMETHING_API ALSPlayerCharacter : public ATP_ThirdPersonCharacter
{
	GENERATED_BODY()

public:
	ALSPlayerCharacter();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
// Stat Section 캐릭터 스탯 컴포넌트 포인터 선언
protected:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAcess = "True"))
	TObjectPtr<class ULSCharacterStatComponent> Stat;

// UI Widjet Section 위젯 컴포넌트 포인터 선언
protected:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAcess = "True"))
	TObjectPtr<class UWidgetComponent> HpBar;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

	void Interaction();
};
