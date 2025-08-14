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
#include "Character/Animation/LSPlayerSiJaeAnimInstance.h"
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
        // darkwidget 생성
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
    Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));

            //소켓 붙이기
            FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("WeaponSocket"));
            FRotator SpawnRotation = GetMesh()->GetSocketRotation(TEXT("WeaponSocket"));
            Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

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

void ALSPlayerSiJae::Move(const FInputActionValue& Value)
{
    if (bIsDead)
        return;

    if (!IsLocallyControlled())
        return;

    if (PushedWheelchairCharacter && PushedWheelchairCharacter->bIsBeingPushed)
    {
        FVector2D MovementVector = Value.Get<FVector2D>();
        HandlePusherWheelchairInput(MovementVector);
        return;
    }
    else
    {
        Super::Move(Value);
    }
}

void ALSPlayerSiJae::Jump()
{
    if (PushedWheelchairCharacter && PushedWheelchairCharacter->bIsBeingPushed)
    {
        return;
    }
    else
    {
        Super::Jump();
    }
}

void ALSPlayerSiJae::HandlePusherWheelchairInput(const FVector2D& MovementVector)
{
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    if (FMath::Abs(MovementVector.Y) > 0.1f)
    {
        AddMovementInput(ForwardDirection, MovementVector.Y);
    }

    // 좌우 입력을 회전 처리
    if (FMath::Abs(MovementVector.X) > 0.1f)
    {
        float TurnInput = MovementVector.X * (WheelchairTurnRate / 200.0f); // 값 조정
        AddControllerYawInput(TurnInput);
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
}

void ALSPlayerSiJae::AnimNotify_AttackHitChek()
{
    UE_LOG(LogTemp, Log, TEXT("Animation_AttackHit"));
}

//
//void ALSPlayerSiJae::AnimNotify_PickChek()
//{
//    UE_LOG(LogTemp, Log, TEXT("Animation_PickCheck"));
//}



void ALSPlayerSiJae::WeaponPickUp()
{
    FHitResult OutHitResult;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponPickUp), false, this);
    const float PickupRange = 200.0f;
    const float PickupRadius = 100.0f;

    FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
    FVector End = Start + GetActorForwardVector() * PickupRange;

    bool bHit = GetWorld()->SweepSingleByChannel(
        OutHitResult, Start, End, FQuat::Identity,
        ECC_GameTraceChannel1,
        FCollisionShape::MakeSphere(PickupRadius),
        Params
    );

    if (!bHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("No weapon found."));
        return;
    }

    AWeapon* HitWeapon = Cast<AWeapon>(OutHitResult.GetActor());
    if (!HitWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit actor is not a weapon."));
        return;
    }

    // 손에 붙이기
    HitWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket")
    );

    //HitWeapon->SetOwner(this);

    // 물리 끄기 + 충돌 끄기
    HitWeapon->SetActorEnableCollision(false);

    if (UStaticMeshComponent* WeaponMesh = HitWeapon->FindComponentByClass<UStaticMeshComponent>())
    {
        WeaponMesh->SetSimulatePhysics(false);
        WeaponMesh->SetEnableGravity(false);
    }



    if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(HitWeapon->GetRootComponent()))
    {
        RootComp->SetSimulatePhysics(false);
    }

    UE_LOG(LogTemp, Log, TEXT("Weapon successfully picked up and attached."));
    

   
}

void ALSPlayerSiJae::ServerWeaponPickUp_Implementation()
{
   
}

void ALSPlayerSiJae::MultiWeaponPickUp_Implementation()
{
    
}

void ALSPlayerSiJae::ClientWeaponPickUp_Implementation()
{
}






//void ALSPlayerSiJae::PickUp()
//{
//	Super::PickUp();
//	
//	FHitResult OutHitResult;
//	FCollisionQueryParams Params(SCENE_QUERY_STAT(PickUp), false, this);
//	const float PickupRange = 200.0f;
//	const float PickupRadius = 100.0f;
//	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
//	const FVector End = Start + GetActorForwardVector() * PickupRange;
//	FColor DrawColor;
//
//
//	//아이템 hit 시
//	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(PickupRadius), Params);
//	if (HitDetected)
//	{
//		AActor* HitActor = OutHitResult.GetActor();
//		
//		// MasterItem 픽업 처리
//		AWeapon* HitItem = Cast<AWeapon>(HitActor);
//		if (HitItem)
//		{
//           
//		}
//
//
//		if (!HitItem)
//		{
//            return;
//		}
//
//
//		if (HasAuthority())
//		{
//			
//			HitItem->Destroy();
//		}
//		else
//		{
//			// 클라이언트인 경우: 서버에 삭제 요청
//			ServerPickUp(HitItem);
//
//		}
//
//		// 아이템 제거
//		HitItem->Destroy();
//
//		
//		DrawColor = FColor::Green;
//		return; // 픽업했으면 함수 종료
//	}
//	else
//	{
//		
//	}
//
//}

//
//
//void ALSPlayer::ServerPickUp_Implementation(AMasterItem* TargetItem)
//{
//	if (!TargetItem) return;
//
//	TargetItem->Destroy();
//	MultiPickUp(TargetItem);
//	
//
//
//}
//
//void ALSPlayer::MultiPickUp_Implementation(AActor* TargetItem)
//{
//	ULSPlayerSiJaeAnimInstance* AnimInstance = Cast<ULSPlayerSiJaeAnimInstance>(GetMesh()->GetAnimInstance());
//	if (AnimInstance)
//	{
//		AnimInstance->SetPickUpAnim();
//		UE_LOG(LogTemp, Warning, TEXT("Player Picking ANIMATION SIJAE"));
//	}
//}

