// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/BossNPC.h"
#include "BossNPC/AI/BossNPCAIController.h"

// Sets default values
ABossNPC::ABossNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// AIController 클래스를 설정
	AIControllerClass = ABossNPCAIController::StaticClass();

	// AIController가 자동으로 소환되도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

// Called when the game starts or when spawned
void ABossNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

