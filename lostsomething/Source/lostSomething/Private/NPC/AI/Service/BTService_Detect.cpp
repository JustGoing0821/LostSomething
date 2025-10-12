// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/Service/BTService_Detect.h"
#include "NPC/AI/TestNPCAIController.h"
#include "NPC/TestNPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include <Character/Players/LSPlayer.h>
#include <Character/VR/VRPawn.h>
//#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	ATestNPC* TM = Cast<ATestNPC>(ControllingPawn);

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 400.0f;

	// 블랙보드에서 현재 저장된 타겟 가져오기
	//UObject* CurrentTarget = OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATestNPCAIController::Key_Target);
	//ATP_ThirdPersonCharacter* ExistingTarget = Cast<ATP_ThirdPersonCharacter>(CurrentTarget);

	// 감지된 오브젝트 목록
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			APawn* Pawn = Cast<APawn>(OverlapResult.GetActor());
			if (Pawn && Pawn->GetController()->IsPlayerController())
			{
				ALSPlayer* TargetPlayer = Cast<ALSPlayer>(Pawn);
				AVRPawn* TargetVRPlayer = Cast<AVRPawn>(Pawn);
				if (TargetPlayer&& !TM->GetIsDead() || TargetVRPlayer)
				{
					TM->SetShouldChase(true);
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATestNPCAIController::Key_Target, Pawn);
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldChase"), true);
				}
				else 
				{
					TM->SetShouldChase(false);
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATestNPCAIController::Key_Target, nullptr);
					OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldChase"), false);
				}
\
				return;
			}
		}
	}

	TM->SetShouldChase(false);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATestNPCAIController::Key_Target, nullptr);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("bShouldChase"), false);
	
	//DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);

}
