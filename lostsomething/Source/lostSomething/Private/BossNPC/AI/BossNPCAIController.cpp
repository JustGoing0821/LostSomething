// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/BossNPCAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <BossNPC/BossNPC.h>
#include "lostSomething.h"


const FName ABossNPCAIController::Key_Phase = FName("Phase");
const FName ABossNPCAIController::Key_CurrentHP = FName("CurrentHP");

ABossNPCAIController::ABossNPCAIController()
{
	//AIController의 BT와 BB를 연결
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/BossNPC/BluePrints/BB_BossNPC.BB_BossNPC'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/BossNPC/BluePrints/BT_BossNPC.BT_BossNPC'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void ABossNPCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	UBlackboardComponent* RawBlackboardPtr = Blackboard.Get();

	if (UseBlackboard(BBAsset, RawBlackboardPtr))
	{
		ABossNPC* BossNPC = Cast<ABossNPC>(GetPawn());
		if (BossNPC)
		{
			float CurrentHP = 100.0f;
			LS_LOG(LogLS, Log, TEXT("BossNPCController OnPossess : %f"), CurrentHP)
			Blackboard->SetValueAsFloat(Key_CurrentHP, CurrentHP);
		}

		Blackboard->SetValueAsEnum(Key_Phase, static_cast<uint8>(EPhaseType::Phase1));
		Blackboard->GetValueAsEnum(Key_Phase);

		RunBehaviorTree(BTAsset);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UseBlackboard failed!"));
	}
}

void ABossNPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossNPCAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr == BehaviorTreeComponent) return;

	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}

void ABossNPCAIController::ChangedHP()
{
	ABossNPC* BossNPC = Cast<ABossNPC>(GetPawn());
	if (BossNPC)
	{
		float CurrentHP = BossNPC->GetHP();
		LS_LOG(LogLS, Log, TEXT("BossNPCController : %f"), CurrentHP)
		Blackboard->SetValueAsFloat(Key_CurrentHP, CurrentHP);
	}
}
