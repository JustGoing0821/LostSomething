// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/LSHpComponent.h"

// Sets default values for this component's properties
ULSHpComponent::ULSHpComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;

}


void ULSHpComponent::BeginPlay()
{
	Super::BeginPlay();

	// MaxHp로 초기화
	CurrentHp = MaxHp;
	UE_LOG(LogTemp, Warning, TEXT("hpcomponent :: HP Component initialized with HP: %.1f"), CurrentHp);

}

void ULSHpComponent::SetHp(float NewHp)
{
	CurrentHp = NewHp;
	OnHpChanged.Broadcast(CurrentHp);
	UE_LOG(LogTemp, Warning, TEXT("hp component : HP changed: %f"), CurrentHp);



}

