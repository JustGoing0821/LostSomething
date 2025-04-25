// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Anim/Notify/AnimNotify_Attack.h"
#include "Interface/TestNPCAttackInterface.h"
#include <NPC/Anim/TestNPCAnimIns.h>

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ITestNPCAttackInterface* AttackPawn = Cast<ITestNPCAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
