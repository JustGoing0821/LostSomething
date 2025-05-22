// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Game/LTGameMode.h"
#include "lostSomething.h"
#include "Quest/LSQuestManager.h"
#include "LevelTest/Player/LTPlayerController.h"

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
				LSPlayerController->CharacterChoice = ECharacterChoice::SiJae;
				DefaultPawnClass = SiJaePawnClass;
			}
			else
			{
				LSPlayerController->CharacterChoice = ECharacterChoice::IJae;
				DefaultPawnClass = IJaePawnClass;
			}
			FString EnumString = StaticEnum<ECharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->CharacterChoice)).ToString();
			LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}
		else
		{
			if (bIsSiJaeServer)
			{
				LSPlayerController->CharacterChoice = ECharacterChoice::IJae;
				DefaultPawnClass = IJaePawnClass;
			}
			else
			{
				LSPlayerController->CharacterChoice = ECharacterChoice::SiJae;
				DefaultPawnClass = SiJaePawnClass;
			}
			FString EnumString = StaticEnum<ECharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->CharacterChoice)).ToString();
			LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}
	}

	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
	return ResultController;
}

void ALTGameMode::QuestStart()
{
	QuestManager->QuestStart();
}

void ALTGameMode::QuestComplete()
{
	QuestManager->QuestComplete();
}
