// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VR/VRPlayer.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"

// Sets default values
AVRPlayer::AVRPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // 카메라 컴포넌트 생성
    VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
    VRCamera->SetupAttachment(GetRootComponent()); // Root에 붙이기
    VRCamera->bUsePawnControlRotation = false;

    // 왼손 Motion Controller
    LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
    LeftController->SetupAttachment(GetRootComponent());
    LeftController->SetTrackingSource(EControllerHand::Left); // 왼손 지정

    // 오른손 Motion Controller
    RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
    RightController->SetupAttachment(GetRootComponent());
    RightController->SetTrackingSource(EControllerHand::Right);

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