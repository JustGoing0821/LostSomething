// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/VendingMachine/LSVendingMachine.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LevelTest/Player/LTPlayerController.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Game/LSGameMode.h"
#include "Puzzle/VendingMachine/LSVendingMachineManager.h"

ALSVendingMachine::ALSVendingMachine()
{
	bReplicates = true;

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
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/SpendingMachines/SM_SpendingMachine_02.SM_SpendingMachine_02"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}


	//Material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_VendingRed.M_VendingRed"));
	if (RedMaterialRef.Object)
	{
		MeshMaterials.Add(EVendingMachineColor::Red, RedMaterialRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GreenMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_VendingGreen.M_VendingGreen"));
	if (GreenMaterialRef.Object)
	{
		MeshMaterials.Add(EVendingMachineColor::Green, GreenMaterialRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_VendingBlue.M_VendingBlue"));
	if (BlueMaterialRef.Object)
	{
		MeshMaterials.Add(EVendingMachineColor::Blue, BlueMaterialRef.Object);
	}
	MeshComponent->SetMaterial(0, MeshMaterials[EVendingMachineColor::Red]);


	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSVendingMachineSiJae"));
	ScriptAssetNameIJae = FName(TEXT("LSVendingMachineIJae"));

	//Quest
	bisCorrectMachine = false;


	//Puzzle
	VendingMachineColorSets.Add({ EVendingMachineColor::Red, EVendingMachineColor::Green, EVendingMachineColor::Blue });
	VendingMachineColorSets.Add({ EVendingMachineColor::Red, EVendingMachineColor::Blue, EVendingMachineColor::Green });
	VendingMachineColorSets.Add({ EVendingMachineColor::Green, EVendingMachineColor::Red, EVendingMachineColor::Blue });
	VendingMachineColorSets.Add({ EVendingMachineColor::Green, EVendingMachineColor::Blue, EVendingMachineColor::Red });
	VendingMachineColorSets.Add({ EVendingMachineColor::Blue, EVendingMachineColor::Red, EVendingMachineColor::Green });
	VendingMachineColorSets.Add({ EVendingMachineColor::Blue, EVendingMachineColor::Green, EVendingMachineColor::Red });

	MachineNumber = 0;
}

void ALSVendingMachine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSVendingMachine, CurrentVendingMachineColor);
}


void ALSVendingMachine::InteractionProcessSiJae()
{
	//const TArray<FString>& Scripts = InteractionScriptDataSiJae->GetInteractionScripts(CurrentQuest);
	//for (FString Script : Scripts)
	//{
	//	LS_LOG(LogLS, Log, TEXT("Interaction Script : %s"), *Script);
	//}

	if (HasAuthority())
	{
		PuzzleCheck();
	}
	else
	{
		ServerRPCPuzzleCheck();
	}
}

void ALSVendingMachine::InteractionProcessIJae()
{
	//Test Log Code
	//const TArray<FString>& Scripts = InteractionScriptDataIJae->GetInteractionScripts(CurrentQuest);
	//for (FString Script : Scripts)
	//{
	//	LS_LOG(LogLS, Log, TEXT("Interaction Script : %s"), *Script);
	//}

	//if (HasAuthority())
	//{
	//	PuzzleCheck();
	//}
	//else
	//{
	//	ServerRPCPuzzleCheck();
	//}

	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("IJae can't interact with this"));
}

void ALSVendingMachine::SetVisibleSiJae()
{
	Super::SetVisibleSiJae();

	MeshComponent->SetMaterial(0, MeshMaterials[EVendingMachineColor::Red]);
}

void ALSVendingMachine::SetVisibleIJae()
{
	Super::SetVisibleIJae();

	MeshComponent->SetMaterial(0, MeshMaterials[CurrentVendingMachineColor]);
}

void ALSVendingMachine::BindVendingMachine(ALSVendingMachineManager* InVendingMachineManager)
{
	InVendingMachineManager->OnVMPhaseChanged.AddUObject(this, &ALSVendingMachine::SetMachineColor);
	InVendingMachineManager->OnVMPuzzleEnd.AddUObject(this, &ALSVendingMachine::MulticastRPCOnQuesetClear);
}

void ALSVendingMachine::SetMachineColor(EVendingMachineColor InAnswerColor, int32 InCurrentColorSet)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	CurrentVendingMachineColor = VendingMachineColorSets[InCurrentColorSet][MachineNumber];

	if (CurrentVendingMachineColor == InAnswerColor)
	{
		bisCorrectMachine = true;
	}
	else
	{
		bisCorrectMachine = false;
	}

	if (HasAuthority())
	{
		MulticastRPCChangeVisible();
	}
}

void ALSVendingMachine::PuzzleCheck()
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (bisCorrectMachine)
	{
		OnVMPuzzleCheck.Execute(true);
	}
	else
	{
		OnVMPuzzleCheck.Execute(false);
	}
}

void ALSVendingMachine::OnQuesetClear()
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	OnVMPuzzleCheck.Unbind();
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetMaterial(0, MeshMaterials[EVendingMachineColor::Red]);
}

void ALSVendingMachine::ServerRPCPuzzleCheck_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	PuzzleCheck();
}

void ALSVendingMachine::MulticastRPCOnQuesetClear_Implementation()
{
	OnQuesetClear();
}
