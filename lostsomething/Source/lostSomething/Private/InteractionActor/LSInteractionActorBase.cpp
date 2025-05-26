// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/LSInteractionActorBase.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Engine/AssetManager.h"
#include "Game/LSGameMode.h"
#include "Quest/LSQuestManager.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"

// Sets default values
ALSInteractionActorBase::ALSInteractionActorBase()
{
	bReplicates = true;

	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSInteractionBase"));
	ScriptAssetNameIJae = FName(TEXT("LSInteractionBase"));

	//Interaction
	CurrentQuest = ELSInteractionEnum::Quest1;
	bSiJaeCanInteraction = false;
	bIJaeCanInteraction = false;
}

void ALSInteractionActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSInteractionActorBase, CurrentQuest);
}

void ALSInteractionActorBase::PostInitializeComponents()
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
			GameMode->GetQuestManager()->OnInteractionChange.AddUObject(this, &ALSInteractionActorBase::SetCurrentQuest);
		}
	}
}

// Called when the game starts or when spawned
void ALSInteractionActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	//Set Owner
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
						LS_LOG(LogLS, Log, TEXT("%s"), TEXT("OwnerSetted"));
						break;
					}
				}
			}
		), 1.0f, false, 3.0f);
	}

	//SiJae IJae Difference Setting
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
		if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
		{
			SetVisibleSiJae();
		}
		else if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::IJae)
		{
			SetVisibleIJae();
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}
}

void ALSInteractionActorBase::InteractionProcess(APlayerController* InPlayerController)
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
		if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
		{
			if (bSiJaeCanInteraction)
			{
				InteractionProcessSiJae();
			}
			else
			{
				LS_LOG(LogLS, Error, TEXT("Cannot do InteractionProcess"));
			}
		}
		else if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::IJae)
		{
			if (bIJaeCanInteraction)
			{
				InteractionProcessIJae();
			}
			else
			{
				LS_LOG(LogLS, Error, TEXT("Cannot do InteractionProcess"));
			}
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}
}

void ALSInteractionActorBase::SetCurrentQuest(ELSInteractionEnum InCurrentQuest)
{
	CurrentQuest = InCurrentQuest;
}

void ALSInteractionActorBase::ChangeVisible()
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
		if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
		{
			SetVisibleSiJae();
		}
		else if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::IJae)
		{
			SetVisibleIJae();
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}
}

void ALSInteractionActorBase::MulticastRPCChangeVisible_Implementation()
{
	ChangeVisible();
}

