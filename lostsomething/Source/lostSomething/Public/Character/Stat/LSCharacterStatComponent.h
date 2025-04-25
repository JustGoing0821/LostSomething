// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LSCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /* Current Hp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOSTSOMETHING_API ULSCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULSCharacterStatComponent();
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetHP(float NewHP);

public:
	FORCEINLINE float GetMaxHP() { return MaxHP; }
	FORCEINLINE float GetCurrentHP() { return CurrentHP; }
	float ApplyDamage(float InDamage);

protected:
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHP;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHP;


		
};
