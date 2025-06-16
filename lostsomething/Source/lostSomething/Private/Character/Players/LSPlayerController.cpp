// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerController.h"
#include "lostSomething.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "UserInterface/LSQuestWidget.h"
#include "Net/UnrealNetwork.h"


ALSPlayerController::ALSPlayerController()
{
	//HUD
	static ConstructorHelpers::FClassFinder<ULSHUDWidget> LSHUDWidgetRef(TEXT("/Game/Players/UI/WBP_LSHUD.WBP_LSHUD_C"));
	if (LSHUDWidgetRef.Class)
	{
		LSHUDWidgetClass = LSHUDWidgetRef.Class;
	}

	//Quest
	static ConstructorHelpers::FClassFinder<ULSQuestWidget> NEHUDWidgetRef(TEXT("/Game/UI/Quest/WBP_QuestWidget.WBP_QuestWidget_C"));
	if (NEHUDWidgetRef.Class)
	{
		QuestWidgetClass = NEHUDWidgetRef.Class;
	}

	//Script
	static ConstructorHelpers::FClassFinder<ULSScriptWidget> ScriptWidgetRef(TEXT("/Game/UI/Script/BP_ScriptWidget.BP_ScriptWidget_C"));
	if (ScriptWidgetRef.Class)
	{
		ScriptWidgetClass = ScriptWidgetRef.Class;
	}
}

void ALSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	if (IsLocalController() && LSHUDWidgetClass)
	{
		LSHUDWidget = CreateWidget<ULSHUDWidget>(this, LSHUDWidgetClass);
		if (LSHUDWidget)
		{
			LSHUDWidget->AddToViewport();
		}
	}

	if (IsLocalController() && QuestWidgetClass)
	{
		QuestWidget = CreateWidget<ULSQuestWidget>(this, QuestWidgetClass);
		if (QuestWidget)
		{
			QuestWidget->AddToViewport();
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("QuestWidget WidgetSetted."));
		}
	}

	if (IsLocalController() && ScriptWidgetClass)
	{
		ScriptWidget = CreateWidget<ULSScriptWidget>(this, ScriptWidgetClass);
		if (ScriptWidget)
		{
			ScriptWidget->AddToViewport();
			UpdateScriptWidget("");
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("ScriptWidget WidgetSetted."));
		}
	}
}

void ALSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSPlayerController, CharacterChoice);
}

void ALSPlayerController::SelectNextSlot()
{
	if (LSHUDWidget)
	{
		int32 CurrentSlot = LSHUDWidget->GetSelectedSlot();
		int32 NextSlot = CurrentSlot + 1;

		LSHUDWidget->ChangeSlot(NextSlot);

		UE_LOG(LogTemp, Warning, TEXT("Selected next slot: %d -> %d"), CurrentSlot, NextSlot);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LSHUDWidget is null in SelectNextSlot"));
	}
}

void ALSPlayerController::SelectPreviousSlot()
{
	if (LSHUDWidget)
	{
		int32 CurrentSlot = LSHUDWidget->GetSelectedSlot();
		int32 PreviousSlot = CurrentSlot - 1;

		LSHUDWidget->ChangeSlot(PreviousSlot);

		UE_LOG(LogTemp, Warning, TEXT("Selected previous slot: %d -> %d"), CurrentSlot, PreviousSlot);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LSHUDWidget is null in SelectPreviousSlot"));
	}
}

void ALSPlayerController::UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum)
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s Begin"), *EnumString);

	if (IsLocalController())
	{
		QuestWidget->UpdateQuestWidget(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
	else
	{
		ClientRPCUpdateQuestWidget(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s ClientRPCUpdateQuestWidget called"), *EnumString);
	}
}

void ALSPlayerController::UpdateScriptWidget(const FString& ScriptText)
{
	if (IsLocalController())
	{
		if (ScriptWidget)
		{
			ScriptWidget->UpdateScriptWidget(ScriptText);
			//ScriptWidget->ShowScriptWidget();
			//LS_LOG(LogLS, Log, TEXT("UpdateScriptWidget Called"));
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("%s"), TEXT("No ScriptWidget"));
		}
	}
	else if (HasAuthority())
	{
		ClientRPCUpdateScriptWidget(ScriptText);
	}
}

void ALSPlayerController::ClientRPCUpdateQuestWidget_Implementation(FLSQuestData InQuestData)
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (this->IsLocalController())
	{
		QuestWidget->UpdateQuestWidget(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
}

void ALSPlayerController::ClientRPCUpdateScriptWidget_Implementation(const FString& ScriptText)
{
	if (IsLocalController())
	{
		if (ScriptWidget)
		{
			ScriptWidget->UpdateScriptWidget(ScriptText);
			//ScriptWidget->ShowScriptWidget();
			//LS_LOG(LogLS, Log, TEXT("UpdateScriptWidget Called"));
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("%s"), TEXT("No ScriptWidget"));
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("Not LocalController"));
	}
}

void ALSPlayerController::SelectSlot(int32 SlotIndex)
{
	if (LSHUDWidget)
	{
		// ½½·Ô ÀÎµ¦½º À¯È¿¼º °Ë»ç
		if (SlotIndex >= 0 && SlotIndex <= 4) // 0~4 ½½·Ô
		{
			LSHUDWidget->ChangeSlot(SlotIndex);
			UE_LOG(LogTemp, Warning, TEXT("Direct slot selection: %d"), SlotIndex);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid slot index: %d"), SlotIndex);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LSHUDWidget is null in SelectSlot"));
	}
}