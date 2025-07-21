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

	static ConstructorHelpers::FClassFinder<ULSDeathWidget> DeathWidgetRef(TEXT("/Game/Players/UI/WBP_DeathWidget.WBP_DeathWidget_C"));
	if (DeathWidgetRef.Class)
	{
		DeathWidgetClass = DeathWidgetRef.Class;
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


	if (IsLocalController() && DeathWidgetClass)
	{
		DeathWidget = CreateWidget<ULSDeathWidget>(this, DeathWidgetClass);
		if (DeathWidget)
		{
			// 생성만 하고 화면에는 추가x(숨김 상태)
			LS_LOG(LogLS, Log, TEXT("%s"), TEXT("DeathWidget Created."));
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

//Death
void ALSPlayerController::ShowDeathWidget()
{
	if (IsLocalController() && DeathWidget)
	{
		DeathWidget->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("Death widget shown"));
	}
}

void ALSPlayerController::HideDeathWidget()
{
	if (IsLocalController() && DeathWidget)
	{
		DeathWidget->RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("Death widget hidden"));
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
