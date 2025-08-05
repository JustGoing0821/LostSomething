// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VR/VRPlayer.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // RootComponent는 ACharacter의 CapsuleComponent 사용
    GetCapsuleComponent()->InitCapsuleSize(40.f, 88.f);

    // VROrigin 생성 (HMD 기준점)
    VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
    VROrigin->SetupAttachment(GetCapsuleComponent());

    // 카메라
    VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
    VRCamera->SetupAttachment(VROrigin);
    VRCamera->bUsePawnControlRotation = false;

    // 왼손 MotionController (Grip + Tracking)
    LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
    LeftController->SetupAttachment(VROrigin);
    LeftController->SetTrackingSource(EControllerHand::Left);

    // 왼손 SkeletalMesh (손 메시)
    HandLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandLeft"));
    HandLeft->SetupAttachment(LeftController); // MotionController 바로 밑

    // 오른손 MotionController (Grip + Tracking)
    RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
    RightController->SetupAttachment(VROrigin);
    RightController->SetTrackingSource(EControllerHand::Right);

    // 오른손 SkeletalMesh (손 메시)
    HandRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandRight"));
    HandRight->SetupAttachment(RightController); // MotionController 바로 밑

}

// Called when the game starts or when spawned
void AVRPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(VRMappingContext, 0);
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AVRPlayer::Move);
        EIC->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &AVRPlayer::Turn);
    }
}

void AVRPlayer::Move(const FInputActionValue& Value)
{
    const FVector2D Input = Value.Get<FVector2D>();
    if (Controller)
    {
        const FRotator YawRotation(0, VRCamera->GetComponentRotation().Yaw, 0);

        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Input.Y);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Input.X);
    }
}

void AVRPlayer::Turn(const FInputActionValue& Value)
{
    float AxisValue = Value.Get<float>();
    AddControllerYawInput(AxisValue * 2.0f);
}

void AVRPlayer::Grab_Left(const FInputActionValue& Value)
{
}

void AVRPlayer::Grab_Right(const FInputActionValue& Value)
{
}

void AVRPlayer::Toggle_Left(const FInputActionValue& Value)
{
}

void AVRPlayer::Toggle_Right(const FInputActionValue& Value)
{
}
