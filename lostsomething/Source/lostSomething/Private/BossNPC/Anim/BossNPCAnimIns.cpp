// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Anim/BossNPCAnimIns.h"
#include <BossNPC/BossNPC.h>

FName UBossNPCAnimIns::GetMontageSectionName(int32 Section)
{
    return FName(*FString::Printf(TEXT("Attack%d"), Section));
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

void UBossNPCAnimIns::MontagePlay(UAnimMontage* Montage)
{
    //UE_LOG(LogTemp, Warning, TEXT("UBossNPCAnimIns::MontagePlay(UAnimMontage* Montage)"));
    if (Montage && !Montage_IsPlaying(Montage))
    {
        Montage_Play(Montage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Montage is nullptr in UTestNPCAnimIns"));
    }
}

void UBossNPCAnimIns::JumpToMontageSection(int32 NewSection, UAnimMontage* Montage)
{
    Montage_JumpToSection(GetMontageSectionName(NewSection), Montage);
}

void UBossNPCAnimIns::PlayRandomMontageSection(UAnimMontage* Montage)
{
    //UE_LOG(LogTemp, Warning, TEXT("UBossNPCAnimIns::PlayRandomMontageSection(UAnimMontage* Montage)"));
    if (!Montage)
    {
        UE_LOG(LogTemp, Warning, TEXT("Montage is null"));
        return;
    }

    int32 MaxSections = 8; // 실제 섹션 개수로 바꾸기
    int32 RandomSection = FMath::RandRange(1, MaxSections); // 0부터 MaxSections-1 사이 랜덤 숫자

    // 선택한 섹션으로 점프
    JumpToMontageSection(RandomSection, Montage);

    // 몽타주 재생 (필요시)
    MontagePlay(Montage);
}