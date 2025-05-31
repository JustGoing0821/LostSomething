// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTPlayerController.h"
#include "lostSomething.h"
#include "LevelTest/Game/LTGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/LSQuestManager.h"
#include "Net/UnrealNetwork.h"
#include "UserInterface/LSQuestWidget.h"


ALTPlayerController::ALTPlayerController()
{
	//Quest
	static ConstructorHelpers::FClassFinder<ULSQuestWidget> NEHUDWidgetRef(TEXT("/Game/UI/Quest/WBP_QuestWidget.WBP_QuestWidget_C"));
	if (NEHUDWidgetRef.Class)
	{
		QuestWidgetClass = NEHUDWidgetRef.Class;
	}
}

void ALTPlayerController::BeginPlay()
{
	if (IsLocalController() && QuestWidgetClass)
	{
		QuestWidget = CreateWidget<ULSQuestWidget>(this, QuestWidgetClass);
		if (QuestWidget)
		{
			QuestWidget->AddToViewport();
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("WidgetSetted."));
		}
	}

	/*if (IsLocalController())
	{
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("WidgetBinding Begin."));
		ALTGameMode* GameMode = Cast<ALTGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			ALSQuestManager* QuestManager = GameMode->GetQuestManager();
			if (QuestManager)
			{
				QuestManager->OnQuestStart.AddUObject(this, &ALTPlayerController::UpdateQuestWidget);
				LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Widget Update Binded"));
				FLSQuestData CurrentQuest = QuestManager->GetCurrentQuest();
				UpdateQuestWidget(CurrentQuest, CurrentQuest.CurrentQuestEnum);
			}
		}
	}*/
}

ELSCharacterChoice ALTPlayerController::GetCharacterChoice()
{
	return CharacterChoice;
}

void ALTPlayerController::SetCharacterChoice(ELSCharacterChoice InCharacterChoice)
{
	CharacterChoice = InCharacterChoice;
}

void ALTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALTPlayerController, CharacterChoice);
}

void ALTPlayerController::UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum)
{
	//Enum LogÀÛ¾÷
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Begin"), *EnumString);

	if (this->IsLocalController())
	{
		QuestWidget->UpdateQuestWidget(InQuestData);
		LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
}
