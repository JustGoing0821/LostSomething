// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VR/Component/GrabComponent.h"
#include "InputCoreTypes.h" // EControllerHand 정의 포함
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Haptics/HapticFeedbackEffect_Base.h>

// Sets default values for this component's properties
UGrabComponent::UGrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UGrabComponent::BeginPlay()
{
	Super::BeginPlay();

    /*&*/

    SetShouldSimulateOnDrop();

    USceneComponent* ParentComp = GetAttachParent();
    if (ParentComp)
    {
        UPrimitiveComponent* PrimitiveParent = Cast<UPrimitiveComponent>(ParentComp);
        if (PrimitiveParent)
        {
            PrimitiveParent->SetCollisionProfileName(FName("PhysicsActor"));
        }
    }
}

bool UGrabComponent::TryGrab(UMotionControllerComponent* MotionController)
{
    if (bIsHeld)
    {
        MotionControllerRef = MotionController;
        OnGrabbed.Broadcast();

        //Haptic 진동 주는 코드 여기 추가

        bIsHeld = true;
    }
    else
    {
        bIsHeld = false;
    }

    return true;
}

bool UGrabComponent::TryRelease()
{
	return false;
}

void UGrabComponent::SetShouldSimulateOnDrop()
{
    USceneComponent* ParentComp = GetAttachParent();

    if (ParentComp)
    {
        // 부모가 UPrimitiveComponent인지 검사 (다이나믹 캐스트)
        UPrimitiveComponent* PrimitiveParent = Cast<UPrimitiveComponent>(ParentComp);
        if (PrimitiveParent)
        {
            // 부모가 물리 시뮬레이션 중인지 확인
            bool bIsSimulating = PrimitiveParent->IsAnySimulatingPhysics();

            if(bIsSimulating)
                bSimulateOnDrop = true;
        }
    }
}

void UGrabComponent::SetPrimitiveCompPhysics(bool bSimulate)
{
    USceneComponent* ParentComp = GetAttachParent();
    if (ParentComp)
    {
        UPrimitiveComponent* PrimitiveParent = Cast<UPrimitiveComponent>(ParentComp);
        if (PrimitiveParent)
        {
            PrimitiveParent->SetSimulatePhysics(bSimulate);
        }
    }
}

EControllerHand UGrabComponent::GetHeldByHand()
{
    if (!MotionControllerRef) // 모션컨트롤러 참조 유효성 체크
    {
        return EControllerHand::Right;
    }

    FName MotionSourceName = MotionControllerRef->MotionSource;

    bool bIsLeftGrip = UKismetMathLibrary::EqualEqual_NameName(MotionSourceName, FName("LeftGrip"));
    GrabType = bIsLeftGrip ? EControllerHand::Left : EControllerHand::Right;

    return GrabType;
}

// Called every frame
void UGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

