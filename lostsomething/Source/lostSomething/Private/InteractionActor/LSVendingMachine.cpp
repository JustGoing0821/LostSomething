// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/LSVendingMachine.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LevelTest/Player/LTPlayerController.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Game/LSGameMode.h"

ALSVendingMachine::ALSVendingMachine()
{
	//Collision
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent = CollisionBox;


	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}


	//Material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedItemMaterialRef(TEXT("/Script/Engine.Material'/Game/Level/InteractionActor/Materials/M_Red.M_Red'"));
	if (RedItemMaterialRef.Object)
	{
		WrongMaterial = RedItemMaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PuppleItemMaterialRef(TEXT("/Script/Engine.Material'/Game/Level/InteractionActor/Materials/M_Pupple.M_Pupple'"));
	if (PuppleItemMaterialRef.Object)
	{
		SiJaeMaterial = PuppleItemMaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueItemMaterialRef(TEXT("/Script/Engine.Material'/Game/Level/InteractionActor/Materials/M_Blue.M_Blue'"));
	if (BlueItemMaterialRef.Object)
	{
		CorrectMaterial = BlueItemMaterialRef.Object;
	}

	MeshComponent->SetMaterial(0, WrongMaterial);


	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSVendingMachineSiJae"));
	ScriptAssetNameIJae = FName(TEXT("LSVendingMachineIJae"));

	//Quest
	bisCorrectMachine = false;
}

void ALSVendingMachine::InteractionProcessSiJae()
{
	const TArray<FString>& Scripts = InteractionScriptDataSiJae->GetInteractionScripts(CurrentQuest);
	for (FString Script : Scripts)
	{
		LS_LOG(LogLS, Log, TEXT("Interaction Script : %s"), *Script);
	}
	if (CurrentQuest == ELSInteractionEnum::Quest1)
	{
		if (bisCorrectMachine)
		{
			if (HasAuthority())
			{
				QuestClear();
			}
			else
			{
				ServerRPCQuestClear();
			}
		}
		else
		{
			LS_LOG(LogLS, Log, TEXT("Wrong Machine"));
		}
	}
	else if (CurrentQuest == ELSInteractionEnum::Quest2)
	{
		if (bisCorrectMachine)
		{
			if (HasAuthority())
			{
				QuestClear();
			}
			else
			{
				ServerRPCQuestClear();
			}
		}
		else
		{
			LS_LOG(LogLS, Log, TEXT("Wrong Machine"));
		}
	}
	else
	{
		return;
	}
}

void ALSVendingMachine::InteractionProcessIJae()
{
	//Test Log Code
	const TArray<FString>& Scripts = InteractionScriptDataIJae->GetInteractionScripts(CurrentQuest);
	for (FString Script : Scripts)
	{
		LS_LOG(LogLS, Log, TEXT("Interaction Script : %s"), *Script);
	}
}

void ALSVendingMachine::SetVisibleSiJae()
{
	Super::SetVisibleSiJae();

	MeshComponent->SetMaterial(0, SiJaeMaterial);
}

void ALSVendingMachine::SetVisibleIJae()
{
	Super::SetVisibleIJae();

	if (bisCorrectMachine)
	{
		MeshComponent->SetMaterial(0, CorrectMaterial);
	}
	else
	{
		MeshComponent->SetMaterial(0, WrongMaterial);
	}
}

void ALSVendingMachine::QuestClear()
{
	ALSGameMode* GameMode = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->QuestComplete();
}

void ALSVendingMachine::ServerRPCQuestClear_Implementation()
{
	QuestClear();
}
