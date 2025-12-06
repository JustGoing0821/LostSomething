// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/LSInteractionTrigger.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Components/BoxComponent.h"
#include "Physics/LSCollisionProfile.h"
#include "Game/LSGameMode.h"
#include "Quest/LSQuestManager.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSScriptWidgetInterface.h"

// Sets default values
ALSInteractionTrigger::ALSInteractionTrigger()
{
	bReplicates = true;

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	RootComponent = TriggerComponent;
	TriggerComponent->SetBoxExtent(FVector(100, 100, 100));
	TriggerComponent->SetCollisionProfileName(CPROFILE_LSTRIGGER);

	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSInteractionBase"));
	ScriptAssetNameIJae = FName(TEXT("LSInteractionBase"));

	//Block
	BlockDeactivateEnum = ELSInteractionEnum::Quest0;

	BlockBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockBox"));
	BlockBox->SetupAttachment(RootComponent);
	BlockBox->SetBoxExtent(FVector(50, 100, 100));
	BlockBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ViewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewMesh"));
	ViewMesh->SetupAttachment(RootComponent);
	ViewMesh->SetCollisionProfileName(TEXT("NoColision"));
}

void ALSInteractionTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Script Section
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("LSScriptData"), Assets);

	if (0 < Assets.Num())
	{
		for (const FPrimaryAssetId& AssetId : Assets)
		{
			if (AssetId.PrimaryAssetName == ScriptAssetNameSiJae)
			{
				FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
				//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

				if (AssetPtr.IsPending())
				{
					AssetPtr.LoadSynchronous();
				}
				InteractionScriptDataSiJae = Cast<ULSInteractionScriptData>(AssetPtr.Get());
				ensure(InteractionScriptDataSiJae);
				bSiJaeCanInteraction = true;
				break;
			}
		}
		if (!bSiJaeCanInteraction)
		{
			LS_LOG(LogLS, Error, TEXT("InteractionScriptDataSiJae Not Found"));
		}

		for (const FPrimaryAssetId& AssetId : Assets)
		{
			//Test Asset
			if (AssetId.PrimaryAssetName == ScriptAssetNameIJae)
			{
				FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
				//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

				if (AssetPtr.IsPending())
				{
					AssetPtr.LoadSynchronous();
				}
				InteractionScriptDataIJae = Cast<ULSInteractionScriptData>(AssetPtr.Get());
				ensure(InteractionScriptDataIJae);
				bIJaeCanInteraction = true;
				break;
			}
		}
		if (!bIJaeCanInteraction)
		{
			LS_LOG(LogLS, Error, TEXT("InteractionScriptDataIJae Not Found"));
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("ScriptAssetName Not Found"));
	}


	//GameMode Delegate Binding
	if (HasAuthority())
	{
		ALSGameMode* GameMode = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->GetQuestManager()->OnInteractionChange.AddUObject(this, &ALSInteractionTrigger::SetCurrentQuest);
		}
	}
}

void ALSInteractionTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSInteractionTrigger, CurrentQuest);
}

// Called when the game starts or when spawned
void ALSInteractionTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ALSInteractionTrigger::OnTriggerBeginOverlap);

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
						//LS_LOG(LogLS, Log, TEXT("Owner Setted."));
						break;
					}
				}
			}
		), 1.0f, false, 2.0f);
	}

	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSInteractionTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		//Enum LogÀÛ¾÷
		FString EnumString = StaticEnum<ELSInteractionEnum>()->GetNameByValue(static_cast<int64>(CurrentQuest)).ToString();
		LS_LOG(LogLSls, Log, TEXT("CurrentQuest : %s"), *EnumString);

		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		AController* OverlapController = OverlapCharacter->GetController();
		if (OverlapCharacter)
		{
			ILSCharacterChoiceInterface* CharacterChoice = Cast<ILSCharacterChoiceInterface>(OverlapController);
			if (CharacterChoice)
			{
				ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(OverlapController);
				FString Script = "";
				if (CharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
				{
					Script = InteractionScriptDataSiJae->GetInteractionScripts(CurrentQuest)[0];
				}
				else
				{
					Script = InteractionScriptDataIJae->GetInteractionScripts(CurrentQuest)[0];
				}
				ScriptController->UpdateScriptWidget(Script);
				LS_LOG(LogLSls, Log, TEXT("%s"), *Script);
			}
		}
	}
}

void ALSInteractionTrigger::SetCurrentQuest(ELSInteractionEnum InCurrentQuest)
{
	CurrentQuest = InCurrentQuest;
}

void ALSInteractionTrigger::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSInteractionTrigger::OnQuestChange);
		}
	}
}

void ALSInteractionTrigger::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	if (BlockDeactivateEnum == ELSInteractionEnum::Quest0) return;

	if (InQuestEnum < BlockDeactivateEnum)
	{
		MulticastRPCPuzzleActivate();
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
	}
}

void ALSInteractionTrigger::PuzzleActivate()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	BlockBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ViewMesh->SetVisibility(true);
}

void ALSInteractionTrigger::PuzzleDeactivate()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	BlockBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ViewMesh->SetVisibility(false);
}

void ALSInteractionTrigger::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSInteractionTrigger::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}



