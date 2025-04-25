// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Anim/TestNPCAnimIns.h"
#include "NPC/TestNPC.h"

UTestNPCAnimIns::UTestNPCAnimIns()
{
	CurrentPawnSpeed = 0.0f;
}

void UTestNPCAnimIns::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();//현재 폰을 받고
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();// 이동속도 받기
        ATestNPC* NPCCharacter = Cast<ATestNPC>(Pawn);
        if (CurrentPawnSpeed > 10.f && NPCCharacter->bIsAttacking) // 걷기 이상이면 공격 멈춤
        {
            if (Montage_IsPlaying(AttackMontage))
            {
                //Montage_Stop(0.2f);
                NPCCharacter->CheckShouldStopMontage(); // 상태도 정리
            }
        }
	}
}

void UTestNPCAnimIns::AttackMontagePlay()
{
    
    if (AttackMontage && !Montage_IsPlaying(AttackMontage))
    {
        Montage_Play(AttackMontage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackMontage is nullptr in UTestNPCAnimIns"));
    }

}

void UTestNPCAnimIns::DamageMontagePlay()
{
    if (DamageMontage && !Montage_IsPlaying(DamageMontage))
    {
        Montage_Play(DamageMontage);
    }
}

void UTestNPCAnimIns::JumpToAttackMontageSection(int32 NewSection)
{
    Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UTestNPCAnimIns::AnimNotify_NextAttackCheck()
{
    UE_LOG(LogTemp, Warning, TEXT("AnimNotify_NextAttackCheck()"));
    auto Pawn = TryGetPawnOwner();
    ATestNPC* NPCCharacter = Cast<ATestNPC>(Pawn);
    if (NPCCharacter)
    {
        NPCCharacter->bIsComboCheckWindowOpen = true;
    }
}

void UTestNPCAnimIns::AnimNotify_EndDamage()
{
    auto Pawn = TryGetPawnOwner();
    ATestNPC* NPCCharacter = Cast<ATestNPC>(Pawn);
    NPCCharacter->SetDespawn();
}

void UTestNPCAnimIns::AnimNotify_EndAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("UTestNPCAnimIns::AnimNotify_EndAttack()"));
    auto Pawn = TryGetPawnOwner();
    ATestNPC* NPCCharacter = Cast<ATestNPC>(Pawn);
    NPCCharacter->NotifyComboActionEnd();

}

FName UTestNPCAnimIns::GetAttackMontageSectionName(int32 Section)
{
    return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

