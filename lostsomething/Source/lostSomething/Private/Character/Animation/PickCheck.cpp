// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/PickCheck.h"
#include "Character/Players/LSPlayer.h"
//#include "PickCheck.h"

void UPickCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (ALSPlayer* Player = Cast<ALSPlayer>(MeshComp->GetOwner()))
    {
        Player->PickUpCore();
    }
    UE_LOG(LogTemp, Warning, TEXT("AnimNotify_PickCheck fired_class"));
}

