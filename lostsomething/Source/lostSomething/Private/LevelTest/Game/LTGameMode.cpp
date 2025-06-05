// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Game/LTGameMode.h"
#include "lostSomething.h"
#include "Quest/LSQuestManager.h"
#include "LevelTest/Player/LTPlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "UserInterface/LSQuestWidget.h"
#include "Game/LSGameInstance.h"

ALTGameMode::ALTGameMode()
{
	// Player Character Class Initialize
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Level/TestPlayer/BP_LTPlayer.BP_LTPlayer_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> IJaePawnClassRef(TEXT("/Game/Level/TestPlayer/BP_LTPlayerIJae.BP_LTPlayerIJae_C"));
	if (IJaePawnClassRef.Class != NULL)
	{
		IJaePawnClass = IJaePawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> SiJaePawnClassRef(TEXT("/Game/Level/TestPlayer/BP_LTPlayerSiJae.BP_LTPlayerSiJae_C"));
	if (SiJaePawnClassRef.Class != NULL)
	{
		SiJaePawnClass = SiJaePawnClassRef.Class;
	}

	//Quest System
	QuestManager = CreateDefaultSubobject<ALSQuestManager>(TEXT("Quest"));

	//Player Controller Class
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/lostSomething.LTPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("PlayerControllerClassRef Not Found"));
	}

	bIsSiJaeServer = true;
	CurrentPlayerCount = 0;

}

void ALTGameMode::BeginPlay()
{

}

APlayerController* ALTGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	APlayerController* ResultController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	ALTPlayerController* LSPlayerController = Cast<ALTPlayerController>(ResultController);
	if (LSPlayerController)
	{
		if (LSPlayerController->GetName() == TEXT("LTPlayerController_0"))
		{
			if (bIsSiJaeServer)
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
				DefaultPawnClass = SiJaePawnClass;
			}
			else
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
				DefaultPawnClass = IJaePawnClass;
			}
			CurrentPlayerCount++;

			FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
			LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}
		else
		{
			if (bIsSiJaeServer)
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
				DefaultPawnClass = IJaePawnClass;
			}
			else
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
				DefaultPawnClass = SiJaePawnClass;
			}
			CurrentPlayerCount++;

			FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
			LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}

		//Quest Widget Update Bind
		QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALTPlayerController::UpdateQuestWidget);
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("UpdateQuestWidget Binded"));
	}

	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
	return ResultController;
}

void ALTGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//LS_LOG(LogLS, Log, TEXT("Begin"));

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			//Quest Start
			if (CurrentPlayerCount == 2)
			{
				QuestStart();
			}
		}
	), 1, false, 3.0f);
}

void ALTGameMode::QuestStart()
{
	QuestManager->QuestStart();
}

void ALTGameMode::QuestComplete()
{
	QuestManager->QuestComplete();
}
