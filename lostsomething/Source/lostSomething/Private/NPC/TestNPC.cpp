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

	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::TakeDamage"));
	Damage();
	SetIsDead(true);
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

void ATestNPC::DespawnMontage()
{
	ServerDespawnMontage();
}

void ATestNPC::ServerDespawnMontage_Implementation()
{
	MultiDespawnMontage();
}

void ATestNPC::MultiDespawnMontage_Implementation()
{
	if (UTestNPCAnimIns* AnimIns = Cast<UTestNPCAnimIns>(GetMesh()->GetAnimInstance()))
	{
		if (AnimIns)
		{
			AnimIns->DespawnMontagePlay();
		}
	}
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
	//LS_LOG(LogLS, Log, TEXT("ServerAttackHitCheck"));

	//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::ServerAttackHitCheck()"));
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (HitDetected)
	{
		//LS_LOG(LogLS, Log, TEXT("HitDetected"));
		//UE_LOG(LogTemp, Warning, TEXT("Multi : HitDetected == true"));
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
	//SetLifeSpan(4.0f);
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	if (ATestNPCAIController* AICon = Cast<ATestNPCAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("Target"))); // 또는 "PlayerActor" 같은 키
			
			FVector OppositeDirection;

			if (Target)
			{
				// 1. 내 위치에서 Target 방향 벡터 구하고 반대 방향으로 회전
				FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
				ToTarget.Z = 0.0f;
				ToTarget.Normalize();
				OppositeDirection = -ToTarget;
			}
			else
			{
				// 타겟이 없을 경우: 현재 바라보는 방향의 반대 방향으로
				FVector CurrentForward = GetActorForwardVector();
				CurrentForward.Z = 0.0f;  // Z축 제거 (수평 이동만)
				CurrentForward.Normalize();
				OppositeDirection = -CurrentForward;
			}

			// 공통 처리: 방향 설정 및 이동
			FRotator NewRot = OppositeDirection.Rotation();
			SetActorRotation(NewRot); // 뒤돌기

			// 1400유닛 떨어진 위치 계산
			FVector EscapeDestination = GetActorLocation() + OppositeDirection * 1400.0f;

			// AI 이동 실행
			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalLocation(EscapeDestination);
			MoveRequest.SetAcceptanceRadius(50.0f); // 5.0f에서 50.0f로 증가

			FNavPathSharedPtr NavPath;
			FPathFollowingRequestResult Result = AICon->MoveTo(MoveRequest, &NavPath);

			// 이동 결과 확인 (디버깅용)
			if (Result.Code != EPathFollowingRequestResult::RequestSuccessful)
			{
				UE_LOG(LogTemp, Warning, TEXT("MoveTo failed: %d"), (int32)Result.Code);
			}
		}
		else
		{
			// BlackboardComponent가 없는 경우에도 기본 동작 수행
			FVector CurrentForward = GetActorForwardVector();
			CurrentForward.Z = 0.0f;
			CurrentForward.Normalize();
			FVector OppositeDirection = -CurrentForward;

			FRotator NewRot = OppositeDirection.Rotation();
			SetActorRotation(NewRot);

			FVector EscapeDestination = GetActorLocation() + OppositeDirection * 1400.0f;

			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalLocation(EscapeDestination);
			MoveRequest.SetAcceptanceRadius(50.0f);

			FNavPathSharedPtr NavPath;
			FPathFollowingRequestResult Result = AICon->MoveTo(MoveRequest, &NavPath);
		}
	}
	FTimerHandle DelayTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DelayTimerHandle,           // 타이머 핸들
		this,                       // 대상 객체
		&ATestNPC::DespawnMontage,  // 실행할 함수
		4.0f,                      // 4초
		false                      // 반복하지 않음 (한 번만 실행)
	);
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
			//UE_LOG(LogTemp, Warning, TEXT("ATestNPC::MultiDamage_Implementation()"));
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
