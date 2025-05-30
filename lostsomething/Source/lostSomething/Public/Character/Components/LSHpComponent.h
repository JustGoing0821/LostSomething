// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSHpComponent.generated.h"


// HP 변경 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float, NewHp);

//HpZero 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHpZeroDelegate, float, ZeroHp);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOSTSOMETHING_API ULSHpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULSHpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
public:
	// HP 관련 함수
	float GetHp() const { return CurrentHp; }
	void SetHp(float NewHp);


	FOnHpChangedDelegate OnHpChanged;
	FOnHpZeroDelegate OnHpZero;


private:

	//함수 변수 
	UPROPERTY(EditAnyWhere)
	float MaxHp = 100.0f;

	UPROPERTY(EditAnyWhere)
	float CurrentHp = 0.f;

	
};
