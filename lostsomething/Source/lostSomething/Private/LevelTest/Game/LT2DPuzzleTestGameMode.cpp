// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Game/LT2DPuzzleTestGameMode.h"
#include "lostSomething.h"
#include "Puzzle/Controller/LS2DPuzzleController.h"

ALT2DPuzzleTestGameMode::ALT2DPuzzleTestGameMode()
{
	//Player Controller Class
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/lostSomething.LS2DPuzzleController"));
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

APlayerController* ALT2DPuzzleTestGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* ResultController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	
	ALS2DPuzzleController* LSPlayerController = Cast<ALS2DPuzzleController>(ResultController);
	if (LSPlayerController)
	{
		if (LSPlayerController->GetName() == TEXT("LS2DPuzzleController_0"))
		{
			if (bIsSiJaeServer)
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
				//DefaultPawnClass = SiJaePawnClass;
			}
			else
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
				//DefaultPawnClass = IJaePawnClass;
			}
			//CurrentPlayerCount++;

			FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
			LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}
		else
		{
			if (bIsSiJaeServer)
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
				//DefaultPawnClass = IJaePawnClass;
			}
			else
			{
				ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
				LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
				//DefaultPawnClass = SiJaePawnClass;
			}
			//CurrentPlayerCount++;

			FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
			LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}

		//Quest Widget Update Bind
		//QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALTPlayerController::UpdateQuestWidget);
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("UpdateQuestWidget Binded"));
	}


	return ResultController;
}
