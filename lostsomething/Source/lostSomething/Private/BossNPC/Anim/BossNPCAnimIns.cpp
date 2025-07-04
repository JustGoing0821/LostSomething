// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Anim/BossNPCAnimIns.h"
#include <BossNPC/BossNPC.h>

FName UBossNPCAnimIns::GetMontageSectionName(int32 Section)
{
    return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void UBossNPCAnimIns::MontageStop(UAnimMontage* Montage, bool bInterrupted)
{
    Montage_Stop(0.25f, Montage);
}

void UBossNPCAnimIns::AnimNotify_AOEAttack()
{
    //UE_LOG(LogTemp, Warning, TEXT("UBossNPCAnimIns::AnimNotify_AOEAttack()"));
    auto Pawn = TryGetPawnOwner();
    ABossNPC* NPCCharacter = Cast<ABossNPC>(Pawn);
    NPCCharacter->EnterPhase1();
}

void UBossNPCAnimIns::AnimNotify_ObsAttack()
{
    auto Pawn = TryGetPawnOwner();
    ABossNPC* NPCCharacter = Cast<ABossNPC>(Pawn);
    NPCCharacter->EnterPhase2();
}

void UBossNPCAnimIns::AnimNotify_MazeAttack()
{
    auto Pawn = TryGetPawnOwner();
    ABossNPC* NPCCharacter = Cast<ABossNPC>(Pawn);
    NPCCharacter->EnterPhase3();
}

void UBossNPCAnimIns::AnimNotify_NextTiming()
{
    auto Pawn = TryGetPawnOwner();
    ABossNPC* NPCCharacter = Cast<ABossNPC>(Pawn);
    NPCCharacter->SetPhaseStatus(false);
}

float UBossNPCAnimIns::MontagePlay(UAnimMontage* Montage)
{
    //CurrentMontage = Montage;

    if (Montage && !Montage_IsPlaying(Montage))
    {
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UBossNPCAnimIns::MontageStop);
        Montage_SetEndDelegate(EndDelegate, Montage);

        return Montage_Play(Montage);

    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Montage is nullptr or already playing in UBossNPCAnimIns"));
        return 0.f;
    }
}
