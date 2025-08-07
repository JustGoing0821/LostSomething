// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Finding/LSFindingPuzzle.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Quest/LSQuestManager.h"
#include "Physics/LSCollisionProfile.h"


ALSFindingPuzzle::ALSFindingPuzzle()
{
	bReplicates = true;

	//Collision
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	CollisionBox->SetBoxExtent(FVector(50.0f, 40.0f, 100.0f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = CollisionBox;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	//MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/Cartels/SM_Cartel_02.SM_Cartel_02"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> TrueMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_CartelB.M_CartelB"));
	if (TrueMaterialRef.Object)
	{
		CorrectMaterial = TrueMaterialRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> WrongMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_CartelR.M_CartelR"));
	if (WrongMaterialRef.Object)
	{
		IncorrectMaterial = WrongMaterialRef.Object;
	}

	bIsCorrectPuzzle = false;
	MeshComponent->SetMaterial(1, IncorrectMaterial);
	PuzzleActivateEnum = ELSInteractionEnum::Quest11;
}

void ALSFindingPuzzle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSFindingPuzzle, bIsCorrectPuzzle);
}

void ALSFindingPuzzle::BeginPlay()
{
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSFindingPuzzle::InteractionProcessSiJae(APlayerController* InPlayerController)
{
	PuzzleCheck(InPlayerController);
}

void ALSFindingPuzzle::InteractionProcessIJae(APlayerController* InPlayerController)
{
}

void ALSFindingPuzzle::SetVisibleSiJae()
{
}

void ALSFindingPuzzle::SetVisibleIJae()
{
	if (bIsCorrectPuzzle)
	{
		MeshComponent->SetMaterial(1, CorrectMaterial);
	}
	else
	{
		MeshComponent->SetMaterial(1, IncorrectMaterial);
	}
}

void ALSFindingPuzzle::SetPuzzleAnswer(uint8 bInCorrectPuzzle)
{
	if (bInCorrectPuzzle)
	{
		bIsCorrectPuzzle = true;
	}
	else
	{
		bIsCorrectPuzzle = false;
	}

	ChangeVisible();
}

void ALSFindingPuzzle::PuzzleCheck(APlayerController* InPlayerController)
{
	if (bIsCorrectPuzzle)
	{
		OnPuzzleCheck.ExecuteIfBound(true);
	}
	else
	{
		OnPuzzleCheck.ExecuteIfBound(false);
	}
}

void ALSFindingPuzzle::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSFindingPuzzle::OnQuestChange);
		}
	}
}

void ALSFindingPuzzle::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	if (InQuestEnum== PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
	}
}

void ALSFindingPuzzle::PuzzleActivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSFindingPuzzle::PuzzleDeactivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetMaterial(1, IncorrectMaterial);
}

void ALSFindingPuzzle::OnRep_bIsCorrectPuzzle()
{
	//LS_LOG(LogLS, Log, TEXT("Begin. bIsCorrectPuzzle = %d"), bIsCorrectPuzzle);
	ChangeVisible();
}

void ALSFindingPuzzle::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSFindingPuzzle::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}


