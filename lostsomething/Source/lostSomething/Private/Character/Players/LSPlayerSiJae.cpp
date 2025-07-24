// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerSiJae.h"
#include "Character/Players/LSPlayerIJae.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSDarkWidget.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"





void ALSPlayerSiJae::BeginPlay()
{
    Super::BeginPlay();
 
 
    if (IJae == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("SiJae: IJae was not found!"));
    }

    if (IsLocallyControlled())
    {
        //darkwidget 생성, 띄우기ㄴ
        if (DarkWidgetClass)
        {
            DarkWidgetInstance = CreateWidget<ULSDarkWidget>(GetWorld(), DarkWidgetClass);

            if (DarkWidgetInstance)
            {
                DarkWidgetInstance->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("DarkWidget successfully added to viewport"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DarkWidgetClass is not set in ALSPlayerSiJae"));
        }

    }
   
}


bool ALSPlayerSiJae::CanPushWheelchair() const
{
    return true;
}

void ALSPlayerSiJae::Attack() {
    if (bIsDead) return;

    int32 CurrentSelectedSlot = SelectedSlot;
    if (CurrentSelectedSlot >= 0 && CurrentSelectedSlot < ItemInfoArray.Num())
    {
        if (!ItemInfoArray[CurrentSelectedSlot].IsEmpty)
        {
            ThrowItem();
            return;
        }
    }

    if (HasAuthority())
    {
        ProcessAttack();
    }
    else
    {
        ServerProcessAttack();
    } 
}


void ALSPlayerSiJae::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IJae)
    {
        IJae = Cast<ALSPlayerIJae>(UGameplayStatics::GetActorOfClass(GetWorld(), ALSPlayerIJae::StaticClass()));
        if (IJae)
        {
            UE_LOG(LogTemp, Warning, TEXT("SiJae: Found IJae player!"));
        }
    }

    if (IJae != nullptr)
    {
        float Distance = FVector::Dist(GetActorLocation(), IJae->GetActorLocation());
        UE_LOG(LogTemp, Warning, TEXT("SiJae: Distance to IJae = %.2f"), Distance);

        if (DarkWidgetInstance)
        {
            DarkWidgetInstance->SetOpacityByDistance(Distance);
        }
    }


    

    //순회하면서 찾기
    //if (!IJae)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("SiJae: Searching for IJae player..."));

    //    for (TActorIterator<ALSPlayerIJae> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    //    {
    //        if (IsValid(*ActorItr))
    //        {
    //            IJae = *ActorItr;
    //            UE_LOG(LogTemp, Warning, TEXT("SiJae: Found IJae player!"));
    //            break;
    //        }
    //    }

    //    if (!IJae)
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("SiJae: IJae player not found yet"));
    //        return; // IJae가 없으면 거리 계산 안 함
    //    }
    //}

    //if (IJae != nullptr)
    //{
    //    float Distance = FVector::Dist(GetActorLocation(), IJae->GetActorLocation());
    //    UE_LOG(LogTemp, Warning, TEXT("SiJae: Distance to IJae = %.2f"), Distance);
    //}

    
}
