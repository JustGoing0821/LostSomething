// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSStepInteraction.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Interface/LSScriptWidgetInterface.h"


ALSStepInteraction::ALSStepInteraction()
{
	bReplicates = true;

	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSTrainStep"));
	ScriptAssetNameIJae = FName(TEXT("LSTrainStep"));

	//Collision
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	CollisionBox->SetBoxExtent(FVector(50.0f, 40.0f, 100.0f));
	RootComponent = CollisionBox;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	//MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/SpendingMachines/SM_SpendingMachine_02.SM_SpendingMachine_02"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	bIsStepGetted = false;
}

void ALSStepInteraction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSStepInteraction, bIsStepGetted);
}

void ALSStepInteraction::InteractionProcess(APlayerController* InPlayerController)
{
	Super::InteractionProcess(InPlayerController);
}

void ALSStepInteraction::InteractionProcessSiJae(APlayerController* InPlayerController)
{
	ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(InPlayerController);
	FString Script = "";

	Script = InteractionScriptDataSiJae->GetInteractionScripts(CurrentQuest)[1];
	ScriptController->UpdateScriptWidget(Script);

	bIsStepGetted = true;
}

void ALSStepInteraction::InteractionProcessIJae(APlayerController* InPlayerController)
{
	ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(InPlayerController);
	FString Script = "";

	Script = InteractionScriptDataSiJae->GetInteractionScripts(CurrentQuest)[0];
	ScriptController->UpdateScriptWidget(Script);
}

void ALSStepInteraction::MulticastRPCHideStep_Implementation()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
}
