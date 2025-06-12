// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/TestNPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <NPC/Anim/TestNPCAnimIns.h>
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include <Net/UnrealNetwork.h>
#include "LevelTest/Interface/LTTakeDamageInterface.h"
#include "Interface/LSTakeDamageInterface.h"
#include <NPC/AI/TestNPCAIController.h>
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "lostSomething.h"

// Sets default values
ATestNPC::ATestNPC()
{
	PrimaryActorTick.bCanEverTick = true;

	// 메시 설정
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Asset/NPC/puppet_animals/crochet_bear/Mesh/SK_crochet_bear.SK_crochet_bear'"));

	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(-20, 0, -86.6), FRotator(0, -90, 0));
	}

	// AIController 클래스를 설정
	AIControllerClass = ATestNPCAIController::StaticClass();

	// AIController가 자동으로 소환되도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	// 이동 속도 설정
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;

	// 초기 공격 상태 설정
	bIsAttacking = false;
	bCanNextCombo = false;
}


// Called when the game starts or when spawned
void ATestNPC::BeginPlay()
{
	Super::BeginPlay();
	if (ATestNPCAIController* PC = Cast<ATestNPCAIController>(GetController()))
	{
		SetOwner(PC);
	}
}

// Called every frame
void ATestNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsComboCheckWindowOpen && bIsAttacking)
	{
		NextComboCheck(); // 이 시점에서 거리 판단
	}

}

// Called to bind functionality to input
void ATestNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/////////////////Attack

void ATestNPC::AttackByAI()
{
	UE_LOG(LogTemp, Warning, TEXT("ATestNPC::AttackByAI()"));
	ServerAttack();
}

void ATestNPC::ServerAttack_Implementation()
{
	MultiAttack();
}

void ATestNPC::MultiAttack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ATestNPC::MultiAttack_Implementation()"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UTestNPCAnimIns* NPCAnimInstance = Cast<UTestNPCAnimIns>(AnimInstance);
	if (NPCAnimInstance && NPCAnimInstance->AttackMontage)
	{
		NPCAnimInstance->AttackMontagePlay();
		NPCAnimInstance->Montage_JumpToSection(FName("Attack1"), NPCAnimInstance->AttackMontage);
		bIsAttacking = true;
		bCanNextCombo = false;
	}
}



void ATestNPC::ComboActionBegin()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UTestNPCAnimIns* NPCAnimInstance = Cast<UTestNPCAnimIns>(AnimInstance);
	if (!NPCAnimInstance || !NPCAnimInstance->AttackMontage) return;

	// 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
	if (AnimInstance->Montage_IsPlaying(NPCAnimInstance->AttackMontage))
	{
		int32 NextSection = 2;
		NPCAnimInstance->JumpToAttackMontageSection(NextSection);
	}
	else
	{
		NPCAnimInstance->AttackMontagePlay();
	}

	// Delegate 연결
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ATestNPC::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, NPCAnimInstance->AttackMontage);
}

void ATestNPC::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)"));

	bIsAttacking = false;

}

void ATestNPC::NotifyComboActionEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("ATestNPC::NotifyComboActionEnd"));

	bIsAttacking = false;
	bCanNextCombo = false;
	bIsComboCheckWindowOpen = false;
	OnAttackFinished.ExecuteIfBound();
}

void ATestNPC::NextComboCheck()
{

	if (!TargetActor) return;

	float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
	if (Distance > MaxComboDistance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too far, stop combo"));

		bCanNextCombo = false;
		bIsAttacking = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Next combo allowed!"));
		bCanNextCombo = true;

		ComboActionBegin(); // 다음 섹션으로 넘어감
	}

	bIsComboCheckWindowOpen = false;
}

void ATestNPC::CheckShouldStopMontage()
{
	ServerStopAttackMontage();
}

void ATestNPC::ServerStopAttackMontage_Implementation()
{
	MultiStopAttackMontage();
}

void ATestNPC::MultiStopAttackMontage_Implementation()
{
	if (UTestNPCAnimIns* AnimIns = Cast<UTestNPCAnimIns>(GetMesh()->GetAnimInstance()))
	{
		if (AnimIns->Montage_IsPlaying(AnimIns->AttackMontage))
		{
			AnimIns->Montage_Stop(0.2f);
			NotifyComboActionEnd();
		}
	}
}

void ATestNPC::AttackHitCheck()
{
	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::AttackHitCheck()"));
	ServerAttackHitCheck();
}

void ATestNPC::ServerAttackHitCheck_Implementation()
{
	LS_LOG(LogLS, Log, TEXT("ServerAttackHitCheck"));

	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::ServerAttackHitCheck()"));
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (HitDetected)
	{
		LS_LOG(LogLS, Log, TEXT("HitDetected"));
		//UE_LOG(LogTemp, Warning, TEXT("Multi : HitDetected == true"));
		FDamageEvent DamageEvent;
		ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		HitResult->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Yellow : FColor::Blue;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);


}

void ATestNPC::MultiAttackHitCheck_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::MultiAttackHitCheck()"));
	LS_LOG(LogLS, Log, TEXT("Begin"));

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel3, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (HitDetected)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Multi : HitDetected == true"));
		FDamageEvent DamageEvent;
		ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		HitResult->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Yellow : FColor::Blue;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

}

void ATestNPC::SetDespawn()
{
	ServerDespawn();
}

void ATestNPC::ServerDespawn_Implementation()
{
	MultiDespawn();
}

void ATestNPC::MultiDespawn_Implementation()
{

	SetLifeSpan(4.0f);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	if (ATestNPCAIController* AICon = Cast<ATestNPCAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("Target"))); // 또는 "PlayerActor" 같은 키
			if (Target)
			{
				// 1. 내 위치에서 Target 방향 벡터 구하고 반대 방향으로 회전
				FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
				ToTarget.Z = 0.0f;
				ToTarget.Normalize();

				FVector OppositeDirection = -ToTarget;
				FRotator NewRot = OppositeDirection.Rotation();
				SetActorRotation(NewRot); // 뒤돌기

				// 2. 4미터 떨어진 위치 계산
				FVector EscapeDestination = GetActorLocation() + OppositeDirection * 1400.0f;

				// 3. AI 이동
				FAIMoveRequest MoveRequest;
				MoveRequest.SetGoalLocation(EscapeDestination);
				MoveRequest.SetAcceptanceRadius(5.0f);

				FNavPathSharedPtr NavPath;
				FPathFollowingRequestResult Result = AICon->MoveTo(MoveRequest, &NavPath);
			}
		}

		AICon->StopAI();
	}

}

float ATestNPC::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::TakeDamage"));
	Damage();
	return 0.0f;
}

void ATestNPC::Damage()
{
	ServerDamage();
}
void ATestNPC::ServerDamage_Implementation()
{
	MultiDamage();
}

void ATestNPC::MultiDamage_Implementation()
{
	if (UTestNPCAnimIns* AnimIns = Cast<UTestNPCAnimIns>(GetMesh()->GetAnimInstance()))
	{
		if (AnimIns)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AnimIns->DamageMontagePlay();"));
			AnimIns->DamageMontagePlay();
		}
	}
}

void ATestNPC::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void ATestNPC::SetMaxWalkSpeed(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

//////////////////// Replicated 변수 할당

void ATestNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestNPC, bIsAttacking);
	DOREPLIFETIME(ATestNPC, bCanNextCombo);
	DOREPLIFETIME(ATestNPC, bIsComboCheckWindowOpen);
	DOREPLIFETIME(ATestNPC, bShouldChase);
}