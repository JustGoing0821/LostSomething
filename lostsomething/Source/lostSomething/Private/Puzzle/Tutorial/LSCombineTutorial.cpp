// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSCombineTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "Quest/LSQuestManager.h"
#include "Net/UnrealNetwork.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSCombineTutorialInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interaction/LSInteractionScriptData.h"


// Sets default values
ALSCombineTutorial::ALSCombineTutorial()
{
	TutorialTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTrigger"));
	RootComponent = TutorialTrigger;
	TutorialTrigger->SetBoxExtent(FVector(100, 100, 100));
	TutorialTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TutorialTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSCombineTutorial::OnTutorialTriggerBeginOverlap);
	TutorialTrigger->OnComponentEndOverlap.AddDynamic(this, &ALSCombineTutorial::OnTutorialTriggerEndOverlap);

	bReplicates = true;
	PuzzleActivateEnum = ELSInteractionEnum::Quest4;

	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSCombineTutorialSiJae"));
	ScriptAssetNameIJae = FName(TEXT("LSCombineTutorialIJae"));
}

void ALSCombineTutorial::PostInitializeComponents()
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

void ALSCombineTutorial::BeginPlay()
{
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSCombineTutorial::OnTutorialTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));
	if (HasAuthority())
	{
		ILSCombineTutorialInterface* WheelchairPlayer = Cast<ILSCombineTutorialInterface>(OtherActor);
		if (WheelchairPlayer)
		{
			if (WheelchairPlayer->isCombining())
			{
				QuestClear();
				TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else
			{
				LS_LOG(LogLS, Warning, TEXT("Being Not Pushed."));
				ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
				ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(OverlapCharacter->GetController());
				ILSCharacterChoiceInterface* CharacterChoice = Cast<ILSCharacterChoiceInterface>(OverlapCharacter->GetController());
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

void ALSCombineTutorial::OnTutorialTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ALSCombineTutorial::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSCombineTutorial::OnQuestChange);
		}
	}
}

void ALSCombineTutorial::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
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

void ALSCombineTutorial::PuzzleActivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSCombineTutorial::PuzzleDeactivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	TutorialTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSCombineTutorial::QuestClear()
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

void ALSCombineTutorial::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSCombineTutorial::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}

