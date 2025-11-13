// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/VendingMachine/LSVendingMachineManager.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Puzzle/VendingMachine/LSVendingMachine.h"
#include "Interface/LSQuestInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Quest/LSQuestManager.h"

ALSVendingMachineManager::ALSVendingMachineManager()
{
	CurrentPhase = ECurrentPhase::NotStarted;
	bReplicates = true;

	//Root Component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	//Collision
	StartButton = CreateDefaultSubobject<UBoxComponent>(TEXT("StartButtonCollision"));
	StartButton->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	StartButton->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//StartButton->SetBoxExtent(FVector(55, 3, 80));
	StartButton->SetupAttachment(RootComponent);

	//Mesh
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMeshComponent"));
	PanelMesh->SetupAttachment(RootComponent);
	PanelMesh->SetCollisionProfileName(TEXT("NoColision"));
	//MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/Cartels/SM_Cartel_02.SM_Cartel_02"));
	if (ItemMeshRef.Object)
	{
		PanelMesh->SetStaticMesh(ItemMeshRef.Object);
	}

	//Material
	PanelColors.Add(EVendingMachineColor::Red, FColor::FromHex(TEXT("DE4D44")));
	PanelColors.Add(EVendingMachineColor::Blue, FColor::FromHex(TEXT("004DD2")));
	PanelColors.Add(EVendingMachineColor::Green, FColor::FromHex(TEXT("007628")));
	Gray = FColor::FromHex(TEXT("9C9C9C"));

	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> RedMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_CartelR.M_CartelR"));
	//if (RedMaterialRef.Object)
	//{
	//	MeshMaterials.Add(EVendingMachineColor::Red, RedMaterialRef.Object);
	//}
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> GreenMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_CartelG.M_CartelG"));
	//if (GreenMaterialRef.Object)
	//{
	//	MeshMaterials.Add(EVendingMachineColor::Green, GreenMaterialRef.Object);
	//}
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_CartelB.M_CartelB"));
	//if (BlueMaterialRef.Object)
	//{
	//	MeshMaterials.Add(EVendingMachineColor::Blue, BlueMaterialRef.Object);
	//}
	//MeshComponent->SetMaterial(1, MeshMaterials[EVendingMachineColor::Red]);

	PuzzleActivateEnum = ELSInteractionEnum::Quest6;
}

void ALSVendingMachineManager::BeginPlay()
{
	Super::BeginPlay();

	UMaterial* BaseMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/Level/Puzzle/VendingMachine/Materials/M_VendingMachinePanel.M_VendingMachinePanel"));

	if (BaseMaterial && PanelMesh)
	{
		PanelMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		PanelMesh->SetMaterial(1, PanelMaterial);

		//PanelMaterial->SetVectorParameterValue(TEXT("BaseColor"), Gray);
	}
	else
	{
		LS_LOG(LogLSls, Error, TEXT("No BaseMaterial or BaseMaterial!!"));
	}

	AActor* TargetActor = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSVendingMachine::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		int32 CurrentMachine = 0;
		for (AActor* FoundActor : FoundActors)
		{
			ALSVendingMachine* VendingMachine = Cast<ALSVendingMachine>(FoundActor);
			if (VendingMachine && VendingMachine->PuzzleActivateEnum == PuzzleActivateEnum)
			{
				VendingMachine->SetMachineNumber(CurrentMachine);
				VendingMachine->BindVendingMachine(this);
				VendingMachine->OnVMPuzzleCheck.BindUObject(this, &ALSVendingMachineManager::PuzzleCheck);
				CurrentMachine++;
			}
		}
		//LS_LOG(LogLS, Log, TEXT("VendingMachines Found : %d"), VendingMachines.Num());
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No ALSVendingMachine"));
	}

	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSVendingMachineManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSVendingMachineManager, CurrentPhase);
	DOREPLIFETIME(ALSVendingMachineManager, CurrentAnswerColor);
	DOREPLIFETIME(ALSVendingMachineManager, PhaseAnswerColors);
}

void ALSVendingMachineManager::ChangeVisible()
{
	if (!PanelMaterial)
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No PanelMaterial!!!"));
		return;
	}

	if (CurrentPhase == ECurrentPhase::Phase1 || CurrentPhase == ECurrentPhase::NotStarted)
	{
		PanelMaterial->SetVectorParameterValue(TEXT("Pase1"), Gray);
		PanelMaterial->SetVectorParameterValue(TEXT("Pase2"), Gray);
		PanelMaterial->SetVectorParameterValue(TEXT("Pase3"), Gray);
		PanelMaterial->SetVectorParameterValue(TEXT("Pase4"), Gray);
	}
	else if (CurrentPhase == ECurrentPhase::Phase2)
	{
		PanelMaterial->SetVectorParameterValue(TEXT("Pase1"), PhaseAnswerColors[0]);
	}
	else if (CurrentPhase == ECurrentPhase::Phase3)
	{
		PanelMaterial->SetVectorParameterValue(TEXT("Pase2"), PhaseAnswerColors[1]);
	}
	else if (CurrentPhase == ECurrentPhase::Phase4)
	{
		PanelMaterial->SetVectorParameterValue(TEXT("Pase3"), PhaseAnswerColors[2]);
	}
	else if (CurrentPhase == ECurrentPhase::Completed)
	{
		PanelMaterial->SetVectorParameterValue(TEXT("Pase4"), PhaseAnswerColors[3]);
	}

	Super::ChangeVisible();
}

void ALSVendingMachineManager::SetVisibleIJae()
{
	if (!PanelMaterial)
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No PanelMaterial!!!"));
		return;
	}

	if (CurrentPhase != ECurrentPhase::NotStarted && CurrentPhase != ECurrentPhase::Completed)
	{
		//MeshComponent->SetMaterial(1, PanelColors[CurrentAnswerColor]);
		PanelMaterial->SetVectorParameterValue(TEXT("Answer"), PanelColors[CurrentAnswerColor]);
	}
	else
	{
		PanelMaterial->SetVectorParameterValue(TEXT("Answer"), Gray);
	}
}

void ALSVendingMachineManager::InteractionProcessSiJae(APlayerController* InPlayerController)
{
	if (HasAuthority())
	{
		StartPhase();
	}
	else
	{
		ServerRPCStartPhase();
	}
}

void ALSVendingMachineManager::InteractionProcessIJae(APlayerController* InPlayerController)
{
	//if (HasAuthority())
	//{
	//	StartPhase();
	//}
	//else
	//{
	//	ServerRPCStartPhase();
	//}
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("IJae can't interact with this"));

}

void ALSVendingMachineManager::StartPhase()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	CurrentPhase = ECurrentPhase::Phase1;

	//Set AnswerColors
	int32 AddCurrentColor = -1;
	for (int32 Num=0 ; Num < 4; Num++)
	{
		int32 NewColor = FMath::RandRange(0, 2);
		while (NewColor == AddCurrentColor)
		{
			NewColor = FMath::RandRange(0, 2);
		}
		AnswerColors.Add(static_cast<ECurrentPhase>(Num+1), static_cast<EVendingMachineColor>(NewColor));
		PhaseAnswerColors.Add(PanelColors[static_cast<EVendingMachineColor>(NewColor)]);
		AddCurrentColor = NewColor;
	}

	//Print AnswerColors Log
	/*
	for (int32 Num = 1; Num < 5; Num++)
	{ 
		EVendingMachineColor Color = AnswerColors[static_cast<ECurrentPhase>(Num)];
		FString EnumString = StaticEnum<EVendingMachineColor>()->GetNameByValue(static_cast<int64>(Color)).ToString();
		LS_LOG(LogLS, Log, TEXT("EVendingMachineColor : %s"), *EnumString);
	}
	*/

	//Set Start Material
	CurrentAnswerColor = AnswerColors[CurrentPhase];
	MulticastRPCChangeVisible();

	//BroadCast
	OnVMPhaseChanged.Broadcast(AnswerColors[CurrentPhase], FMath::RandRange(0, 5));
}

void ALSVendingMachineManager::ProceedPhase()
{
	//Set Answer
	CurrentPhase = static_cast<ECurrentPhase>(static_cast<uint8>(CurrentPhase)+1);
	FString EnumString = StaticEnum<ECurrentPhase>()->GetNameByValue(static_cast<int64>(CurrentPhase)).ToString();
	//LS_LOG(LogLS, Log, TEXT("CurrentPhase : %s"), *EnumString);

	//Set Start Material
	CurrentAnswerColor = AnswerColors[CurrentPhase];
	MulticastRPCChangeVisible();

	//BroadCast
	OnVMPhaseChanged.Broadcast(AnswerColors[CurrentPhase], FMath::RandRange(0, 5));
}

void ALSVendingMachineManager::PuzzleCheck(bool bisPuzzleCorrect)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (HasAuthority())
	{
		if (bisPuzzleCorrect)
		{
			if (CurrentPhase != ECurrentPhase::Phase4)
			{
				ProceedPhase();
			}
			else
			{
				MulticastRPCQuestClear();
			}
		}
		else
		{
			StartPhase();
		}
	}
	else
	{
		if (bisPuzzleCorrect)
		{
			if (CurrentPhase != ECurrentPhase::Phase4)
			{
				ServerRPCProceedPhase();
			}
			else
			{
				ServerRPCQuestClear();
			}
		}
		else
		{
			ServerRPCStartPhase();
		}
	}
}

void ALSVendingMachineManager::QuestClear()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->QuestComplete();
		}
		OnVMPhaseChanged.Clear();
		OnVMPuzzleEnd.Broadcast();
		OnVMPuzzleEnd.Clear();
	}

	CurrentPhase = ECurrentPhase::Completed;
	MulticastRPCChangeVisible();
	StartButton->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSVendingMachineManager::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSVendingMachineManager::OnQuestChange);
		}
	}
}

void ALSVendingMachineManager::PuzzleActivate()
{
	StartButton->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSVendingMachineManager::PuzzleDeactivate()
{
	StartButton->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSVendingMachineManager::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
	}
}

void ALSVendingMachineManager::MulticastRPCQuestClear_Implementation()
{
	QuestClear();
}

void ALSVendingMachineManager::ServerRPCQuestClear_Implementation()
{
	MulticastRPCQuestClear();
}

void ALSVendingMachineManager::ServerRPCProceedPhase_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ProceedPhase();
}

void ALSVendingMachineManager::ServerRPCStartPhase_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	StartPhase();
}

void ALSVendingMachineManager::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSVendingMachineManager::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}
