// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Item/LSItemBase.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/Players/LSPlayerController.h"

// Sets default values
ALSItemBase::ALSItemBase()
{
    PrimaryActorTick.bCanEverTick = false;
    SetActorEnableCollision(true);

   

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = MeshComponent;

    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
    MeshComponent->SetGenerateOverlapEvents(true);


}

// 수정
// LSItemBase.cpp의 OnPickup 함수 수정
void ALSItemBase::OnPickup(APlayerController* Controller)
{
    UE_LOG(LogTemp, Warning, TEXT("ALSItemBase::OnPickup called for item: %s"), *GetName());

    if (!Controller)
    {
        UE_LOG(LogTemp, Error, TEXT("Controller is null in OnPickup"));
        return;
    }

    // 플레이어 컨트롤러로 캐스팅
    ALSPlayerController* LSController = Cast<ALSPlayerController>(Controller);
    if (!LSController)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast to LSPlayerController"));
        return;
    }

    // 아이콘 유효성 검사
    if (!ItemIcon)
    {
        UE_LOG(LogTemp, Error, TEXT("ItemIcon is null for %s"), *GetName());
        // 아이콘이 없더라도 기본 아이콘으로 시도
        // 여기서 기본 아이콘을 사용하거나 다른 방법을 시도할 수 있습니다
    }

  
}


//void ALSItemBase::OnPickup(APlayerController* Controller)
//{
//    if (!Controller) return;
//
//    APawn* PlayerPawn = Controller->GetPawn();
//    if (!PlayerPawn) return;
//
//    // HUD로 전달
//    if (ALSPlayerController* PC = Cast<ALSPlayerController>(Controller))
//    {
//        if (ULSHUDWidget* HUD = PC->GetLSHUDWidget())  
//        {
//            HUD->SetItemIconToSlot(ItemIcon); // 아이콘 전달
//        }
//    }
//
//    Destroy();
//}



void ALSItemBase::OnPickedUp(AActor* Picker)
{
    if (Picker)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s picked %s"), *Picker->GetName(), *GetName());
    }

    UE_LOG(LogTemp, Warning, TEXT("destroy item : %s"), *GetName());

    Destroy();  // 실제 아이템 제거
}

