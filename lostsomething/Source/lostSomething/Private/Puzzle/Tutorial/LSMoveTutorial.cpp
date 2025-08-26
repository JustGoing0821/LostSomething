// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSMoveTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "Interface/LSQuestInterface.h"
#include "Quest/LSQuestManager.h"
#include "Interaction/LSInteractionEnum.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Character/Players/LSCharacterChoice.h"

// Sets default values
ALSMoveTutorial::ALSMoveTutorial()
{
	//TutorialTrigger
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSMoveTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSMoveTutorial::OnTutorialTriggerEndOverlap);

	bReplicates = true;
	PuzzleActivateEnum = ELSInteractionEnum::Quest1;

	//Script
	ScriptAssetNameSiJae = FName(TEXT("LSMoveTutorialSiJae"));
	ScriptAssetNameIJae = FName(TEXT("LSMoveTutorialIJae"));
}

void ALSMoveTutorial::PostInitializeComponents()
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
				break;
			}
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
				break;
			}
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("ScriptAssetName Not Found"));
	}
}

// Called when the game starts or when spawned
void ALSMoveTutorial::BeginPlay()
{
	Super::BeginPlay();
	//LS_LOG(LogLS, Log, TEXT("Begin"));
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSMoveTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));

	if (HasAuthority())
	{
		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		if (OverlapCharacter)
		{
			ILSCharacterChoiceInterface* CharacterChoice = Cast<ILSCharacterChoiceInterface>(OverlapCharacter->GetController());
			if (CharacterChoice)
			{
				CurrentTriggerPlayers++;
				LS_LOG(LogLS, Log, TEXT("CurrentTriggerPlayers = %d"), CurrentTriggerPlayers);
				if (CurrentTriggerPlayers == 2)
				{
					QuestClear();
					return;
				}

				ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(OverlapCharacter->GetController());
				FString Script = "";
				if (CharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
				{
					Script = InteractionScriptDataSiJae->GetInteractionScripts(PuzzleActivateEnum)[0];
				}
				else
				{
					Script = InteractionScriptDataIJae->GetInteractionScripts(PuzzleActivateEnum)[0];
				}
				ScriptController->UpdateScriptWidget(Script);

			}
		}
	}
}

void ALSMoveTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		if (OverlapCharacter)
		{
			ILSScriptWidgetInterface* LSCharacter = Cast<ILSScriptWidgetInterface>(OverlapCharacter->GetController());
			if (LSCharacter)
			{
				CurrentTriggerPlayers--;
				LS_LOG(LogLS, Log, TEXT("CurrentTriggerPlayers = %d"), CurrentTriggerPlayers);
			}
		}
	}
}

void ALSMoveTutorial::BindQuestChange()
{
	if (HasAuthority())
	{
		//LS_LOG(LogLS, Log, TEXT("Begin"));
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSMoveTutorial::OnQuestChange);
		}
	}
}

void ALSMoveTutorial::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
	}
}

void ALSMoveTutorial::PuzzleActivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		//FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}

	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSMoveTutorial::PuzzleDeactivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		//FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}

	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSMoveTutorial::QuestClear()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->QuestComplete();
		}
	}
}

void ALSMoveTutorial::MulticastRPCPuzzleActivate_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	PuzzleActivate();
}

void ALSMoveTutorial::MulticastRPCPuzzleDeactivate_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	PuzzleDeactivate();
}
