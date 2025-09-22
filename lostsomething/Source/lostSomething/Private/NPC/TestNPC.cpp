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
#include <Kismet/GameplayStatics.h>

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

	CurrentHP = MaxHP;
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

float ATestNPC::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	SetHP(GetHP() - DamageAmount);
	Damage();
	if (GetHP() <= 0)
	{
		SetIsDead(true);
		SetDespawn();
	}
		
	return 0.0f;
}

/////////////////Attack

void ATestNPC::AttackStart()
{
	UE_LOG(LogTemp, Warning, TEXT("ATestNPC::AttackByAI()"));
	ServerAttackStart();
}

void ATestNPC::ServerAttackStart_Implementation()
{
	MultiAttackStart();
}

void ATestNPC::MultiAttackStart_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ATestNPC::MultiAttack_Implementation()"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UTestNPCAnimIns* NPCAnimInstance = Cast<UTestNPCAnimIns>(AnimInstance);
	if (NPCAnimInstance && NPCAnimInstance->AttackMontage)
	{
		NPCAnimInstance->MontagePlay(NPCAnimInstance->AttackMontage);
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
		NPCAnimInstance->MontagePlay(NPCAnimInstance->AttackMontage);
	}

	// Delegate 연결
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ATestNPC::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, NPCAnimInstance->AttackMontage);
}

void ATestNPC::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	bIsAttacking = false;
}

void ATestNPC::NotifyComboActionEnd()
{
	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::NotifyComboActionEnd"));

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
		//UE_LOG(LogTemp, Warning, TEXT("Too far, stop combo"));

		bCanNextCombo = false;
		bIsAttacking = false;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Next combo allowed!"));
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
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OutHitResult.GetActor());
		if (HitResult)
		{
			HitResult->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
		}
	}

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Yellow : FColor::Blue;

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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UTestNPCAnimIns* NPCAnimInstance = Cast<UTestNPCAnimIns>(AnimInstance);
	if (!NPCAnimInstance || !NPCAnimInstance->DespawnMontage) return;

	// 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
	if (NPCAnimInstance->DespawnMontage)
	{
		NPCAnimInstance->MontagePlay(NPCAnimInstance->DespawnMontage);
	}
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UTestNPCAnimIns* NPCAnimInstance = Cast<UTestNPCAnimIns>(AnimInstance);
	if (!NPCAnimInstance || !NPCAnimInstance->DamageMontage) return;

	// 몽타주가 재생 중일 경우 섹션 이동, 아니라면 재생
	if (NPCAnimInstance->DamageMontage)
	{
		NPCAnimInstance->MontagePlay(NPCAnimInstance->DamageMontage);
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

void ATestNPC::TMSoundPlay(const FString& SoundType)
{
	ServerTMSoundPlay(SoundType);
}

void ATestNPC::ServerTMSoundPlay_Implementation(const FString& SoundType)
{
	MultiTMSoundPlay(SoundType);
}

void ATestNPC::MultiTMSoundPlay_Implementation(const FString& SoundType)
{
	//LS_LOG(LogLS, Log, TEXT("SoundType : %s"), *SoundType);

	USoundBase* SelectedSound = nullptr;

	if (SoundType == "Idle") SelectedSound = IdleSound;
	else if (SoundType == "Attack") SelectedSound = AttackSound;
	else if (SoundType == "Damage") SelectedSound = DamageSound;
	else if (SoundType == "Chase") SelectedSound = ChaseSound;
	else if (SoundType == "Walk") SelectedSound = WalkSound;
	else if (SoundType == "Run") SelectedSound = RunSound;

	if (!SelectedSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound is nullptr for type %s"), *SoundType);
		return;
	}

	// 핵심: GetWorld() 기반으로 2D 사운드 재생
	UGameplayStatics::PlaySound2D(GetWorld(), SelectedSound, 0.1f);
}
