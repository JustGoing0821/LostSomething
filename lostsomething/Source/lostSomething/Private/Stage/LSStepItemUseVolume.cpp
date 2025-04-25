// Fill out your copyright notice in the Description page of Project Settings.


#include "Stage/LSStepItemUseVolume.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALSStepItemUseVolume::ALSStepItemUseVolume()
{
	// Stage Section
	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	RootComponent = InteractionTrigger;
	InteractionTrigger->SetBoxExtent(FVector(50.0, 50.0f, 50.0f));
	InteractionTrigger->SetCollisionProfileName(TEXT("LSItemTest"));

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	MeshComponent->SetVisibility(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ItemMaterialRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/LevelPrototyping/Materials/MI_Solid_Blue.MI_Solid_Blue'"));
	if (ItemMaterialRef.Object)
	{
		MeshComponent->SetMaterial(0, ItemMaterialRef.Object);
	}

	//Step Install Initialize
	bReplicates = true;
	bIsStepInstalled = false;

}

void ALSStepItemUseVolume::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSStepItemUseVolume, bIsStepInstalled);
}

void ALSStepItemUseVolume::BeginPlay()
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
				LS_LOG(LogLS, Log, TEXT("OwnerSetted."));
			}
		), 2.0f, false, -1.0f);
	}
}

void ALSStepItemUseVolume::InteractionProcess(APlayerController* InPlayerController)
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

void ALSStepItemUseVolume::ServerRPCSetVisibility_Implementation(uint8 bInStepInstalled)
{
	bIsStepInstalled = bInStepInstalled;
	LS_LOG(LogLS, Log, TEXT("Called. bIsStepInstalled : %d"), bIsStepInstalled);
	MulticastRPCSetVisibility();
}

void ALSStepItemUseVolume::MulticastRPCSetVisibility_Implementation()
{
	MeshComponent->SetVisibility(bIsStepInstalled);
	LS_LOG(LogLS, Log, TEXT("Called. bIsStepInstalled : %d"), bIsStepInstalled);
}

