// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerSiJae.h"
#include "Character/Players/LSPlayerIJae.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSDarkWidget.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Players/LSPlayer.h"
#include "Character/Item/Weapon.h"
#include "Components/CapsuleComponent.h"
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



ALSPlayerSiJae::ALSPlayerSiJae()
{
//    Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
// 
//
//    //소켓 붙이기
//    FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("WeaponSocket"));
//    FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("WeaponSocket"));
//
//    //Weapon = GetWorld()->SpawnActor<ASword>(SwordClass, SpawnLocation, SpawnRotation);
//    Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
}




void ALSPlayerSiJae::PostPickUp()
{
    Super::PostPickUp();

    FHitResult OutHitResult;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(PickUp), false, this);
    const float PickupRange = 200.0f;
    const float PickupRadius = 100.0f;
    const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector End = Start + GetActorForwardVector() * PickupRange;
    FColor DrawColor;

    bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(PickupRadius), Params);
    if (HitDetected)
    {
        AActor* HitActor = OutHitResult.GetActor();

        AWeapon* HitItem = Cast<AWeapon>(HitActor);
        if (HitItem)
        {
            FName WeaponSocketName = TEXT("WeaponSocket");
            USkeletalMeshComponent* PlayerMesh = GetMesh();

            if (PlayerMesh->DoesSocketExist(WeaponSocketName))
            {
             
                HitItem->AttachToComponent(
                    PlayerMesh,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                    WeaponSocketName
                );

                // 위치, 회전 조정 (원하면 여기에 추가)
                HitItem->SetActorRelativeLocation(FVector::ZeroVector);
                HitItem->SetActorRelativeRotation(FRotator::ZeroRotator);

                // 들고 있는 무기로 저장해도 됨
                // CurrentWeapon = HitItem;
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Socket %s does not exist!"), *WeaponSocketName.ToString());
            }

            return; 
        }

 

        if (!HitItem)
        {

            return; // 마스터 아이템이 아니면 픽업 무시하고 종료
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
        //UE_LOG(LogTemp, Warning, TEXT("SiJae: Distance to IJae = %.2f"), Distance);

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
