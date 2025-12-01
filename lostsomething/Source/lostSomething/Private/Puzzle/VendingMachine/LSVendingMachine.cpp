// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/VendingMachine/LSVendingMachine.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LevelTest/Player/LTPlayerController.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Game/LSGameMode.h"
#include "Puzzle/VendingMachine/LSVendingMachineManager.h"
#include "Quest/LSQuestManager.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Interface/LSScriptWidgetInterface.h"

ALSVendingMachine::ALSVendingMachine()
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
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GreyMaterialRef(TEXT("/Game/Level/Puzzle/VendingMachine/MaterialInstance/M_VendingGrey.M_VendingGrey"));
	if (GreyMaterialRef.Object)
	{
		GreyMaterial = GreyMaterialRef.Object;
	}
	MeshComponent->SetMaterial(0, GreyMaterial);


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
	PuzzleActivateEnum = ELSInteractionEnum::Quest0;
	DamageAmount = 10.f;
	bisPhaseStart = false;
}

void ALSVendingMachine::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSVendingMachine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSVendingMachine, bisCorrectMachine);
	DOREPLIFETIME(ALSVendingMachine, CurrentVendingMachineColor);
	DOREPLIFETIME(ALSVendingMachine, CurrentInteractController);
	DOREPLIFETIME(ALSVendingMachine, bisPhaseStart);
}


void ALSVendingMachine::InteractionProcess(APlayerController* InPlayerController)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (HasAuthority())
	{
		SetCurrentInteractController(InPlayerController);
	}
	else
	{
		ServerRPCSetCurrentInteractController(InPlayerController);
	}

	Super::InteractionProcess(InPlayerController);
}

void ALSVendingMachine::InteractionProcessSiJae(APlayerController* InPlayerController)
{
	//Script
	TArray<FString> ScriptArray;
	FString Script = "";
	ScriptArray = InteractionScriptDataSiJae->GetInteractionScripts(CurrentQuest);
	if (ScriptArray.Num() == 0)
	{
		Script = "Failed to load Script";
	}
	else if (CurrentQuest == ELSInteractionEnum::Quest2)
	{
		if (!bisCorrectMachine)
		{
			Script = ScriptArray[0];
		}
	}
	else if (CurrentQuest == ELSInteractionEnum::Quest6)
	{
		if (!bisPhaseStart)
		{
			Script = ScriptArray[0];
		}
		else if (!bisCorrectMachine)
		{
			Script = ScriptArray[1];
		}
	}

	ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(InPlayerController);
	if (ScriptController)
	{
		ScriptController->UpdateScriptWidget(Script);
	}


	//Audio
	if (bisCorrectMachine)
	{
		if (CorrectAudioComponent)
		{
			CorrectAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
	else
	{
		if (WrongAudioComponent)
		{
			WrongAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}


	//Puzzle
	if ((CurrentQuest == ELSInteractionEnum::Quest6)&&!bisPhaseStart)
	{
		return;
	}

	if (HasAuthority())
	{
		PuzzleCheck();
	}
	else
	{
		ServerRPCPuzzleCheck();
	}
}

void ALSVendingMachine::InteractionProcessIJae(APlayerController* InPlayerController)
{
	//Script
	TArray<FString> ScriptArray;
	FString Script = "";
	ScriptArray = InteractionScriptDataIJae->GetInteractionScripts(CurrentQuest);
	if (ScriptArray.Num() == 0)
	{
		Script = "Failed to load Script";
	}
	else if (CurrentQuest == ELSInteractionEnum::Quest2)
	{
		if (bisCorrectMachine)
		{
			Script = ScriptArray[0];
		}
		else
		{
			Script = ScriptArray[1];
		}
	}
	else if (CurrentQuest == ELSInteractionEnum::Quest6)
	{
		if (!bisPhaseStart)
		{
			Script = ScriptArray[0];
		}
		else if (bisCorrectMachine)
		{
			Script = ScriptArray[1];
		}
		else
		{
			Script = ScriptArray[2];
		}
	}

	ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(InPlayerController);
	if (ScriptController)
	{
		ScriptController->UpdateScriptWidget(Script);
	}


	//Audio
	if (bisCorrectMachine)
	{
		if (BaseAudioComponent)
		{
			BaseAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
	else
	{
		if (BaseAudioComponent)
		{
			BaseAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
}

void ALSVendingMachine::SetVisibleSiJae()
{
	Super::SetVisibleSiJae();

	//MeshComponent->SetMaterial(0, MeshMaterials[EVendingMachineColor::Red]);
}

void ALSVendingMachine::SetVisibleIJae()
{
	Super::SetVisibleIJae();

	MeshComponent->SetMaterial(0, MeshMaterials[CurrentVendingMachineColor]);
}

void ALSVendingMachine::BindVendingMachine(ALSVendingMachineManager* InVendingMachineManager)
{
	InVendingMachineManager->OnVMPhaseChanged.AddUObject(this, &ALSVendingMachine::SetMachineColor);
}

void ALSVendingMachine::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSVendingMachine::OnQuestChange);
		}
	}
}

void ALSVendingMachine::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	CurrentQuest = InQuestEnum;

	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
		MulticastRPCLSSetMapIcon(true);
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
		MulticastRPCLSSetMapIcon(false);
	}
}

void ALSVendingMachine::PuzzleActivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (PuzzleActivateEnum == ELSInteractionEnum::Quest2)
	{
		ChangeVisible();
	}
}

void ALSVendingMachine::PuzzleDeactivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetMaterial(0, GreyMaterial);
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

	bisPhaseStart = true;

	if (HasAuthority())
	{
		MulticastRPCChangeVisible();
	}
}

void ALSVendingMachine::PuzzleCheck()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (CurrentQuest==ELSInteractionEnum::Quest5&&!bisPhaseStart)
	{
		return;
	}

	if (bisCorrectMachine)
	{
		OnVMPuzzleCheck.Execute(true);
		MulticastRPCSpawnSystem();
	}
	else
	{
		OnVMPuzzleCheck.Execute(false);
		ApplyDamage();
	}
}

void ALSVendingMachine::ApplyDamage()
{
	ILSTakeDamageInterface* LSPlayer = Cast<ILSTakeDamageInterface>(CurrentInteractController->GetPawn());
	if (LSPlayer)
	{
		FDamageEvent DamageEvent;
		LSPlayer->TakeDamage(DamageAmount, DamageEvent, nullptr, this);
	}
}

void ALSVendingMachine::SpawnSystem()
{
	if (!Effect)
	{
		LS_LOG(LogLS, Error, TEXT("Effect is null!"));
		return;
	}

	// Get Actor Location & Rotation
	FVector ActorLocation = GetActorLocation();// -CollisionBox->GetScaledBoxExtent() * FVector(0, 0, 1);
	FRotator ActorRotation = GetActorRotation();

	// Spawn System at Location
	UNiagaraComponent* SpawnedSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		Effect,
		ActorLocation,
		ActorRotation,
		FVector(Scale), // Scale을 FVector로 변환
		true,  // Auto Destroy
		true,  // Auto Activate
		ENCPoolMethod::None,
		true   // Pre Cull Check
	);

	if (SpawnedSystem)
	{
		// Set Niagara Variable (Float) - Scale 설정
		SpawnedSystem->SetNiagaraVariableFloat(FString("Scale"), Scale);

		// Branch 노드 구현 - Change Color 체크
		if (bChangeColor)
		{
			// Set Niagara Variable (LinearColor) - MainColor 설정
			SpawnedSystem->SetNiagaraVariableLinearColor(FString("MainColor"), CustomColor);
		}
	}
}

void ALSVendingMachine::ServerRPCPuzzleCheck_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	PuzzleCheck();
}

void ALSVendingMachine::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSVendingMachine::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}

void ALSVendingMachine::ServerRPCSetCurrentInteractController_Implementation(APlayerController* InPlayerController)
{
	SetCurrentInteractController(InPlayerController);
}

void ALSVendingMachine::MulticastRPCSpawnSystem_Implementation()
{
	SpawnSystem();
}
