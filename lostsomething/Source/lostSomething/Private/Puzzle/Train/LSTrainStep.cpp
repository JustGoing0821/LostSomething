// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrainStep.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LS2DPuzzleGameModeInterface.h"


// Sets default values
ALSTrainStep::ALSTrainStep()
{
	// Stage Section
	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionCollision"));
	RootComponent = InteractionTrigger;
	InteractionTrigger->SetBoxExtent(FVector(65, 25, 80));
	InteractionTrigger->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeScale3D(FVector(1.3f, 0.5f, 0.2f));
	MeshComponent->SetRelativeLocation(FVector(-65.0f, -25.0f, -80.0f));
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
	PuzzleTimer = 5.f;
}

void ALSTrainStep::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrainStep, bIsStepInstalled);
}

void ALSTrainStep::BeginPlay()
{
	Super::BeginPlay();
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (HasAuthority())
	{
		for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
		{
			if (PlayerController && !PlayerController->IsLocalController())
			{
				SetOwner(PlayerController);
				break;
			}
		}
	}

	if (HasAuthority())
	{
		ILS2DPuzzleGameModeInterface* GameMode = Cast<ILS2DPuzzleGameModeInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->Get2DPuzzleClearDelegate().AddUObject(this, &ALSTrainStep::InstallStep);
			GameMode->Get2DPuzzleFailedDelegate().AddUObject(this, &ALSTrainStep::MulticastRPCFailedInstallPuzzle);
		}
	}


	//if (HasAuthority())
	//{
	//	FTimerHandle Handle;
	//	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	//		{
	//			for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	//			{
	//				if (PlayerController && !PlayerController->IsLocalController())
	//				{
	//					SetOwner(PlayerController);
	//					LS_LOG(LogLS, Log, TEXT("Owner Setted."));
	//					break;
	//				}
	//			}
	//		}
	//	), 1.0f, false, 2.0f);
	//}
}

void ALSTrainStep::InteractionProcess(APlayerController* InPlayerController)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ILSCharacterChoiceInterface* LSController = Cast<ILSCharacterChoiceInterface>(InPlayerController);
	if (LSController)
	{
		if (LSController->GetCharacterChoice() == ELSCharacterChoice::SiJae)
		{
			if (HasAuthority())
			{
				//InstallStep();
				StartInstallPuzzle();
			}
			else
			{
				//ServerRPCInstallStep();
				ServerRPCStartInstallPuzzle();
			}
		}
		else
		{
			ILSScriptWidgetInterface* LSScript = Cast<ILSScriptWidgetInterface>(InPlayerController);
			LSScript->UpdateScriptWidget(TEXT("I Can't Install Step"));
			//if (HasAuthority())
			//{
			//	//InstallStep();
			//	StartInstallPuzzle();
			//}
			//else
			//{
			//	//ServerRPCInstallStep();
			//	ServerRPCStartInstallPuzzle();
			//}
		}
	}
}

void ALSTrainStep::InstallStep()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//OnStepInstalled.Execute();

	if (bIsStepInstalled)
	{
		//bIsStepInstalled = false;
	}
	else
	{
		bIsStepInstalled = true;
	}
	//MeshComponent->SetVisibility(bIsStepInstalled);

	MulticastRPCInstallStep();
}

void ALSTrainStep::PuzzleDeactivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSTrainStep::StartInstallPuzzle()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ILS2DPuzzleGameModeInterface* GameMode = Cast<ILS2DPuzzleGameModeInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->Start2DPuzzle(PuzzleTimer);
	}
}

//void ALSTrainStep::ClearInstallPuzzle()
//{
//	InstallStep();
//}

//void ALSTrainStep::FailedInstallPuzzle()
//{
//	//OnStepInstallFailed.ExecuteIfBound();
//	Destroy();
//}

void ALSTrainStep::ServerRPCInstallStep_Implementation()
{
	InstallStep();
}

void ALSTrainStep::MulticastRPCInstallStep_Implementation()
{
	MeshComponent->SetVisibility(bIsStepInstalled);
	//LS_LOG(LogLS, Log, TEXT("Called. bIsStepInstalled : %d"), bIsStepInstalled);
	PuzzleDeactivate();
}

void ALSTrainStep::ServerRPCStartInstallPuzzle_Implementation()
{
	StartInstallPuzzle();
}

void ALSTrainStep::MulticastRPCFailedInstallPuzzle_Implementation()
{
	Destroy();
}

