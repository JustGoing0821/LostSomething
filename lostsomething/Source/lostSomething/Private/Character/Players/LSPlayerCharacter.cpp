// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerCharacter.h"
#include "lostSomething.h"
#include "InputMappingContext.h"
#include "Interface/LSInteractionInterface.h"
#include "Components/WidgetComponent.h"
#include "Character/Stat/LSCharacterStatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSHpBarWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/LSTakeDamageInterface.h"
#include "EnhancedInputComponent.h"
#include <Character/Players/LSPlayerController.h>

ALSPlayerCharacter::ALSPlayerCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	/*	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;*/


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Capsule 루트 컴포넌트. 
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement 움직임 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;


	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Players/Input/IMC_Default.IMC_Default'"));
	if (nullptr != InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Players/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Players/Input/Actions/IA_Look.IA_Look'"));
	if (nullptr != InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Players/Input/Actions/IA_Move.IA_Move'"));
	if (nullptr != InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteractRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Players/Input/Actions/IA_Interaction.IA_Interaction'"));
	if (nullptr != InputActionInteractRef.Object)
	{
		InteractAction = InputActionInteractRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Players/Input/Actions/IA_Atack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}
	

	// Mesh
	GetMesh()->SetupAttachment(RootComponent);


	// Mesh 모델링 위치, 회전 지정 및 애니메이션 모드 지정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoColision"));
	GetMesh()->bOnlyOwnerSee = false;

	//오브젝트 가져와서 매쉬 지정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	
}

void ALSPlayerCharacter::BeginPlay()
{
	/*ALSPlayerController* PlayerController = CastChecked<ALSPlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		
	}*/
	

	//FInputModeGameOnly GameOnlyInputMode;
	//PlayerController->SetInputMode(GameOnlyInputMode);
}

void ALSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	/*APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}*/

	LS_LOG(LogLS, Warning, TEXT("void ALSPlayerCharacter::PossessedBy(AController* NewController)"));


	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	ALSPlayerController* LSPlayerController = Cast<ALSPlayerController>(PlayerController);
	if (LSPlayerController)
	{
		LS_LOG(LogLS, Warning, TEXT("PlayerController"));
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			LS_LOG(LogLS, Warning, TEXT("Subsystem->AddMappingContext(DefaultMappingContext, 0);"));
		}

		FInputModeGameOnly GameOnlyInputMode;
		LSPlayerController->SetInputMode(GameOnlyInputMode);
		LS_LOG(LogLS, Warning, TEXT("PlayerController->SetInputMode(GameOnlyInputMode);"));
	}
	else
	{
		LS_LOG(LogLS, Warning, TEXT("PlayerController - null"));
	}
	
}




void ALSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//이동

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//캐릭터 기본제공 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
		//MOVE,LOOK 매핑
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALSPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALSPlayerCharacter::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALSPlayerCharacter::Interaction);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ALSPlayerCharacter::Attack);
	}
	else
	{
		LS_LOG(LogLS, Warning, TEXT("EnhacndeInput NUll"));
	}

}

void ALSPlayerCharacter::Attack()
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	const float AttackRange = 80.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 10.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		if (HitNPC)
		{
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}
	}
	else
	{
		DrawColor = FColor::Red;
	}
}

void ALSPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	//LS_LOG(LogLS, Log, TEXT("Move Inut"), MovementVector.X, MovementVector.Y);

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);

}

void ALSPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

float ALSPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LS_LOG(LogLS, Warning, TEXT("Take Damage : %f"), DamageAmount);
	return 0.0f;
}



//인터렉션
void ALSPlayerCharacter::Interaction()
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	const float AttackRange = 80.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 10.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	FColor DrawColor;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		ILSInteractionInterface* HitActor = Cast<ILSInteractionInterface>(OutHitResult.GetActor());
		if (HitActor)
		{
			HitActor->InteractionProcess(PlayerController);
			DrawColor = FColor::Green;
		}

		ILSTakeDamageInterface* HitNPC = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		if (HitNPC)
		{
			FDamageEvent DamageEvent;
			HitNPC->TakeDamage(10.0f, DamageEvent, GetController(), this);
			DrawColor = FColor::Blue;
		}
	}
	else
	{
		DrawColor = FColor::Red;
	}

//void ALSPlayerCharacter::Interaction()
//{
//	FHitResult OutHitResult;
//	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
//	const float AttackRange = 80.0f;
//	const float AttackRadius = 50.0f;
//	const float AttackDamage = 10.0f;
//	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
//	const FVector End = Start + GetActorForwardVector() * AttackRange;
//
//	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);
//	
//	if (HitDetected)
//	{
//		ILSInteractionInterface* HitActor = Cast<ILSInteractionInterface>(OutHitResult.GetActor());
//		if (HitActor)
//		{
//			HitActor->InteractionProcess();
//		}
//
//		APlayerController* PlayerController = Cast<APlayerController>(GetController());
//		ILSInteractionInterface* HitActor = Cast<ILSInteractionInterface>(OutHitResult.GetActor());
//		if (HitActor)
//		{
//			HitActor->InteractionProcess(PlayerController);
//		}
//	}


	//디버그 동그라미
//#if enable_draw_debug
//
//	fvector capsuleorigin = start + (end - start) * 0.5f;
//	float capsulehalfheight = attackrange * 0.5f;
//	fcolor drawcolor = hitdetected ? fcolor::green : fcolor::red;
//
//	drawdebugcapsule(getworld(), capsuleorigin, capsulehalfheight, attackradius, frotationmatrix::makefromz(getactorforwardvector()).toquat(), drawcolor, false, 5.0f);
//
//#endif
}



//// Stat Component
//Stat = CreateDefaultSubobject<ULSCharacterStatComponent>(TEXT("Stat"));
//
//// Widjet Component
//HpBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
//HpBar->SetupAttachment(GetMesh());
//HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
//static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
//if (HpBarWidgetRef.Class)
//{
//	HpBar->SetWidgetClass(HpBarWidgetRef.Class);
//	HpBar->SetWidgetSpace(EWidgetSpace::Screen);
//	HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
//	HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//}
