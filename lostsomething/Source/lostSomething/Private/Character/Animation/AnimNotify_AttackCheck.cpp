// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify_AttackCheck.h"
#include "Character/Players/LSPlayer.h"


void UAnimNotify_AttackCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (ALSPlayer* Player = Cast<ALSPlayer>(MeshComp->GetOwner()))
    {
        Player->ProcessAttack();
    }

    UE_LOG(LogTemp, Warning, TEXT("AnimNotify_attackcheck fired_class"));
}
