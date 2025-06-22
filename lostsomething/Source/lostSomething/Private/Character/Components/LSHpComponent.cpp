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
    // 0과 MaxHp 사이로 값 제한
    float ClampedHp = FMath::Clamp(NewHp, 0.0f, MaxHp);

    // 실제로 값이 변경되었는지 확인
    if (CurrentHp != ClampedHp)
    {
        CurrentHp = ClampedHp;
        OnHpChanged.Broadcast(CurrentHp);
        UE_LOG(LogTemp, Warning, TEXT("HP changed: %.1f"), CurrentHp);

        // HP가 0이 되면 델리게이트 호출
        if (CurrentHp <= 0.0f)
        {
            OnHpZero.Broadcast(CurrentHp);
        }
    }

 //   // 0과 MaxHp 사이로 값 제한
 //   NewHp = FMath::Clamp(NewHp, 0.0f, MaxHp);

	//CurrentHp = NewHp;
	//OnHpChanged.Broadcast(CurrentHp);
	////UE_LOG(LogTemp, Warning, TEXT("hp component : HP changed: %f"), CurrentHp);


 //   if (CurrentHp != NewHp)
 //   {
 //       CurrentHp = NewHp;
 //       OnHpChanged.Broadcast(CurrentHp);
 //       UE_LOG(LogTemp, Warning, TEXT("HP changed: %.1f"), CurrentHp);

 //       // HP가 0이 되면 델리게이트 호출
 //       if (CurrentHp <= 0.0f)
 //       {
 //           OnHpZero.Broadcast(CurrentHp);
 //       }
 //   }

}

