// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTPlayerCharacter.h"
#include "lostSomething.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Interface/LSInteractionInterface.h"
#include "LevelTest/Interface/LTTakeDamageInterface.h"
#include "NPC/TestNPC.h"


ALTPlayerCharacter::ALTPlayerCharacter()
{
	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Level/TestPlayer/IMC_Default.IMC_Default'"));
	if (nullptr != InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look'"));
	if (nullptr != InputActionLookRef.Object)
	{
		LookAction = InputActionLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move'"));
	if (nullptr != InputActionMoveRef.Object)
	{
		MoveAction = InputActionMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteractRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Level/TestPlayer/IA_Interaction.IA_Interaction'"));
	if (nullptr != InputActionInteractRef.Object)
	{
		InteractAction = InputActionInteractRef.Object;
	}

	// Capsule Collision
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("LSCollisionTest"));

	// Mesh
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoColision"));
	GetMesh()->bOnlyOwnerSee = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}
}

void ALTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALTPlayerCharacter::Interaction);
	}
}

void ALTPlayerCharacter::Interaction()
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

		ILTTakeDamageInterface* HitNPC = Cast<ILTTakeDamageInterface>(OutHitResult.GetActor());
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

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}

float ALTPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LS_LOG(LogLS, Warning, TEXT("Take Damage : %f"), DamageAmount);
	return 0.0f;
}

bool ALTPlayerCharacter::isCombining()
{
	return true;
}
