// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrainStep.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"

// Sets default values
ALSTrainStep::ALSTrainStep()
{
	// Stage Section
	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	RootComponent = InteractionTrigger;
	InteractionTrigger->SetBoxExtent(FVector(80, 50, 80));
	InteractionTrigger->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeScale3D(FVector(1.6f, 1.0f, 0.2f));
	MeshComponent->SetRelativeLocation(FVector(-80.0f, -50.0f, -80.0f));
	MeshComponent->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ItemMaterialRef(TEXT("/Game/Level/InteractionActor/Materials/M_Blue.M_Blue"));
	if (ItemMaterialRef.Object)
	{
		MeshComponent->SetMaterial(0, ItemMaterialRef.Object);
	}

	//Step Install Initialize
	bReplicates = true;
	bIsStepInstalled = false;

}

void ALSTrainStep::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrainStep, bIsStepInstalled);
}

void ALSTrainStep::BeginPlay()
{
	Super::BeginPlay();

	//Set Owner - 3.0f delay
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
				{
					if (PlayerController && !PlayerController->IsLocalController())
					{
						SetOwner(PlayerController);
						break;
					}
				}
				//LS_LOG(LogLS, Log, TEXT("OwnerSetted."));
			}
		), 2.0f, false, -1.0f);
	}
}

void ALSTrainStep::InteractionProcess(APlayerController* InPlayerController)
{
	if (bIsStepInstalled)
	{
		bIsStepInstalled = false;
	}
	else
	{
		bIsStepInstalled = true;
	}
	MeshComponent->SetVisibility(bIsStepInstalled);

	if (HasAuthority())
	{
		MulticastRPCSetVisibility();
	}
	else
	{
		ServerRPCSetVisibility(bIsStepInstalled);
	}
}

void ALSTrainStep::ServerRPCSetVisibility_Implementation(uint8 bInStepInstalled)
{
	bIsStepInstalled = bInStepInstalled;
	LS_LOG(LogLS, Log, TEXT("Called. bIsStepInstalled : %d"), bIsStepInstalled);
	MulticastRPCSetVisibility();
}

void ALSTrainStep::MulticastRPCSetVisibility_Implementation()
{
	MeshComponent->SetVisibility(bIsStepInstalled);
	LS_LOG(LogLS, Log, TEXT("Called. bIsStepInstalled : %d"), bIsStepInstalled);
}

