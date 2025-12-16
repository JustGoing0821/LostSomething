// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/AI/TestNPCAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <NPC/TestNPC.h>

/*
BT노드에서 Key 값 사용할 때
#include "ATestNPCAIController.h"
OwnerComp->GetBlackboardComponent()->GetValueAsVector(ATestNPCAIController::Key_HomePos)
*/ 

const FName ATestNPCAIController::Key_HomePos = FName("HomePos");
const FName ATestNPCAIController::Key_PatrolPos = FName("PatrolPos");
const FName ATestNPCAIController::Key_Target = FName("Target");
const FName ATestNPCAIController::Key_bShouldChase = FName("bShouldChase"); 
const FName ATestNPCAIController::Key_bIsHit = FName("bIsHit");

ATestNPCAIController::ATestNPCAIController()
{
	//AIController의 BT와 BB를 연결
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/NPC/Blueprints/BB_TestNPC.BB_TestNPC'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/NPC/Blueprints/BT_TestNPC.BT_TestNPC'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void ATestNPCAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		// HomePos 값을 AI의 현재 위치로 초기화
		Blackboard->SetValueAsVector(Key_HomePos, InPawn->GetActorLocation());
		//UE_LOG(LogTemp, Log, TEXT("ANPCAIController: Home Position Set To %s"), *InPawn->GetActorLocation().ToString());

		RunBehaviorTree(BTAsset);
	}
}

void ATestNPCAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestNPCAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr == BehaviorTreeComponent) return;

	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}

 