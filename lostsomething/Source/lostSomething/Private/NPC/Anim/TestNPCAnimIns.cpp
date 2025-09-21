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
        NPCCharacter = Cast<ATestNPC>(Pawn);
        if (CurrentPawnSpeed > 10.f && NPCCharacter->bIsAttacking) // 걷기 이상이면 공격 멈춤
        {
            if (Montage_IsPlaying(AttackMontage))
            {
                //Montage_Stop(0.2f);
                NPCCharacter->CheckShouldStopMontage(); // 상태도 정리
            }
        }
        if (NPCCharacter->GetShouldChase())
        {
            bShouldChase = true;
            NPCCharacter->SetMaxWalkSpeed(350.0f);
        }
        else
        {
            bShouldChase = false;
            NPCCharacter->SetMaxWalkSpeed(150.0f);
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

void UTestNPCAnimIns::DespawnMontagePlay()
{
    if (DespawnMontage && !Montage_IsPlaying(DespawnMontage))
    {
        Montage_Play(DespawnMontage);
    }
}

void UTestNPCAnimIns::JumpToAttackMontageSection(int32 NewSection)
{
    Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UTestNPCAnimIns::AnimNotify_NextAttackCheck()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_NextAttackCheck : nullptr"));
        return;
    }
    NPCCharacter->bIsComboCheckWindowOpen = true;
}

void UTestNPCAnimIns::AnimNotify_EndDamage()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_EndDamage : nullptr"));
        return;
    }
    NPCCharacter->SetDespawn();
}

void UTestNPCAnimIns::AnimNotify_Despawn()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_Despawn : nullptr"));
        return;
    }
    NPCCharacter->Destroy();
}

void UTestNPCAnimIns::AnimNotify_IdleSound()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_IdleSound: NPC is nullptr"));
        return;
    }

    NPCCharacter->TMSoundPlay(TEXT("Idle"));
}

void UTestNPCAnimIns::AnimNotify_DamageSound()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_DamageSound: NPC is nullptr"));
        return;
    }

    NPCCharacter->TMSoundPlay(TEXT("Damage"));
}

void UTestNPCAnimIns::AnimNotify_WalkSound()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_WalkSound: NPC is nullptr"));
        return;
    }

    NPCCharacter->TMSoundPlay(TEXT("Walk"));
}

void UTestNPCAnimIns::AnimNotify_RunSound()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_RunSound: NPC is nullptr"));
        return;
    }

    NPCCharacter->TMSoundPlay(TEXT("Run"));
}

void UTestNPCAnimIns::AnimNotify_EndAttack()
{
    if (!NPCCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimNotify_EndAttack:nullptr"));
        return;
    }
    NPCCharacter->NotifyComboActionEnd();

}

FName UTestNPCAnimIns::GetAttackMontageSectionName(int32 Section)
{
    return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

