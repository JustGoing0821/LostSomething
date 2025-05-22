// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/AI/BossNPCAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


const EPhaseType ABossNPCAIController::Key_Phase = EPhaseType::Phase1;

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
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		// HomePos 값을 AI의 현재 위치로 초기화
		Blackboard->SetValueAsEnum(FName("Key_Phase"), static_cast<uint8>(EPhaseType::Phase2));

		/* 블랙보드 값 가져오는 법
		uint8 PhaseValue = BlackboardComp->GetValueAsEnum(FName("Key_Phase"));
		EPhaseType Phase = static_cast<EPhaseType>(PhaseValue);
		*/

		RunBehaviorTree(BTAsset);
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
