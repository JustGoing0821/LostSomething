// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionActor/LSInteractionActorBase.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Engine/AssetManager.h"
#include "LevelTest/Player/LTPlayerController.h"
#include "Game/LSGameMode.h"
#include "Quest/LSQuestManager.h"

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
						break;
					}
				}
			}
		), 1.0f, false, 3.0f);
	}

	//SiJae IJae Difference Setting
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer)
	{
		ALTPlayerController* LSPlayer = Cast<ALTPlayerController>(LocalPlayer->GetPlayerController(GetWorld()));
		if (LSPlayer)
		{
			if (LSPlayer->CharacterChoice != ECharacterChoice::None)
			{
				FString EnumString = StaticEnum<ECharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayer->CharacterChoice)).ToString();
				//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
				if (LSPlayer->CharacterChoice == ECharacterChoice::SiJae)
				{
					SetVisibleSiJae();
				}
				else
				{
					SetVisibleIJae();
				}
			}
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("LocalPlayer Controller Not Found"));
		}
	}
}

void ALSInteractionActorBase::InteractionProcess(APlayerController* InPlayerController)
{
	ALTPlayerController* PlayerController = Cast<ALTPlayerController>(InPlayerController);
	if (PlayerController)
	{
		if (PlayerController->CharacterChoice == ECharacterChoice::SiJae)
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
		else
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
}

void ALSInteractionActorBase::SetCurrentQuest(ELSInteractionEnum InCurrentQuest)
{
	CurrentQuest = InCurrentQuest;
}

void ALSInteractionActorBase::ChangeVisible()
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer)
	{
		ALTPlayerController* LSPlayer = Cast<ALTPlayerController>(LocalPlayer->GetPlayerController(GetWorld()));
		if (LSPlayer)
		{
			if (LSPlayer->CharacterChoice != ECharacterChoice::None)
			{
				FString EnumString = StaticEnum<ECharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayer->CharacterChoice)).ToString();
				//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
				if (LSPlayer->CharacterChoice == ECharacterChoice::SiJae)
				{
					SetVisibleSiJae();
				}
				else
				{
					SetVisibleIJae();
				}
			}
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("LocalPlayer Controller Not Found"));
		}
	}
}

void ALSInteractionActorBase::MulticastRPCChangeVisible_Implementation()
{
	ChangeVisible();
}

