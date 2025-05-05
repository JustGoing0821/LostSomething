// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Stat/LSCharacterStatComponent.h"

ULSCharacterStatComponent::ULSCharacterStatComponent()
{
	MaxHP = 200.0f;
	SetHP(MaxHP);
}


// Called when the game starts
void ULSCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	SetHP(MaxHP);
}




float ULSCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHP(PrevHp - ActualDamage);
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void ULSCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = FMath::Clamp<float>(NewHP, 0.0f, MaxHP);
	OnHpChanged.Broadcast(CurrentHP);
}

