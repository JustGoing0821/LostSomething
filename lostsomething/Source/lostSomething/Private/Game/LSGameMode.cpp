// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSGameMode.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/LSQuestManager.h"
#include "Character/Players/LSPlayerController.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "UserInterface/LSQuestWidget.h"
#include "Game/LSGameInstance.h"
#include "Level/LSMapVersionControll.h"

ALSGameMode::ALSGameMode()
{
	// Player Character Class Initialize
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Level/TestPlayer/BP_LTPlayer.BP_LTPlayer_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> IJaePawnClassRef(TEXT("/Game/Players/BluePrints/BP_LSPlayerIJae.BP_LSPlayerIJae_C"));
	if (IJaePawnClassRef.Class != NULL)
	{
		IJaePawnClass = IJaePawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> SiJaePawnClassRef(TEXT("/Game/Players/BluePrints/BP_LSPlayerSiJae.BP_LSPlayerSiJae_C"));
	if (SiJaePawnClassRef.Class != NULL)
	{
		SiJaePawnClass = SiJaePawnClassRef.Class;
	}

	//Quest System
	QuestManager = CreateDefaultSubobject<ALSQuestManager>(TEXT("Quest"));

	//Player Controller Class
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/lostSomething.LSPlayerController"));
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

	MapVersions.Add("LSStage1Map1", 0);
	MapVersions.Add("LSStage1Map2", 0);
	MapVersions.Add("LSStage1Map3", 0);

	//2D Section
	bIsSiJaeDragging = false;
}

APlayerController* ALSGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	APlayerController* ResultController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	ULSGameInstance* GameInstance = Cast<ULSGameInstance>(GetGameInstance());

	//Not Used ChoseCharacterMap
	if (GameInstance)
	{
		if (GameInstance->GetServerCharacterChoice() == ELSCharacterChoice::None)
		{
			TestLoginProcess(ResultController);
			return ResultController;
		}
	}

	//Used ChooseCharacterMap
	if (GameInstance)
	{
		ALSPlayerController* LSPlayerController = Cast<ALSPlayerController>(ResultController);
		if (LSPlayerController)
		{
			if (LSPlayerController->GetName() == TEXT("LSPlayerController_0"))
			{
				if (GameInstance->GetServerCharacterChoice() == ELSCharacterChoice::SiJae)
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
				//LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
			}
			else
			{
				if (GameInstance->GetClientCharacterChoice() == ELSCharacterChoice::IJae)
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
				//LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
			}

			//Quest Widget Update Bind
			QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALSPlayerController::UpdateQuestWidget);
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("UpdateQuestWidget Binded"));
		}
	}

	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
	return ResultController;
}

void ALSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//LS_LOG(LogLS, Log, TEXT("Begin"));
}

void ALSGameMode::StartGame()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			//Quest Start
			if (CurrentPlayerCount == 2)
			{
				CheckMapVersion();
				QuestStart();
			}
		}
	), 2.f, false);

	OnStartGame.Broadcast();
}

void ALSGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ALSGameMode::QuestStart()
{
	QuestManager->QuestStart();
}

void ALSGameMode::QuestComplete()
{
	QuestManager->QuestComplete();
}

void ALSGameMode::TransferPlayerLocation(FVector InSijaeLocation, FVector InIjaeLocation)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ALSPlayerController* LSPlayerController = Cast<ALSPlayerController>(Iterator->Get());
		if (LSPlayerController->GetName() == TEXT("LSPlayerController_0"))
		{
			ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
			APawn* Pawn = LSPlayerController->GetPawn(); // 또는 Cast<AMyCharacter>(PC->GetPawn());

			if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
			{
				Pawn->SetActorLocation(InSijaeLocation);
			}
			else
			{
				Pawn->SetActorLocation(InIjaeLocation);
			}
		}
		else
		{
			ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
			APawn* Pawn = LSPlayerController->GetPawn(); // 또는 Cast<AMyCharacter>(PC->GetPawn());

			if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
			{
				Pawn->SetActorLocation(InSijaeLocation);
			}
			else
			{
				Pawn->SetActorLocation(InIjaeLocation);
			}
		}
	}
}

void ALSGameMode::TestLoginProcess(APlayerController* ResultController)
{
	ALSPlayerController* LSPlayerController = Cast<ALSPlayerController>(ResultController);
	if (LSPlayerController)
	{
		if (LSPlayerController->GetName() == TEXT("LSPlayerController_0"))
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
			//LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
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
			//LS_LOG(LogLS, Log, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
		}

		//Quest Widget Update Bind
		QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALSPlayerController::UpdateQuestWidget);
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("UpdateQuestWidget Binded"));
	}
}

void ALSGameMode::BroadcastScript(const FString& InScript)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		APlayerController* PC = Cast<APlayerController>(Actor);
		ILSScriptWidgetInterface* ScriptWidget = Cast<ILSScriptWidgetInterface>(PC);
		if (ScriptWidget)
		{
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
			ScriptWidget->UpdateScriptWidget(InScript);
		}
	}
}

void ALSGameMode::CheckMapVersion()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	FString MapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (MapVersions.Contains(MapName))
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSMapVersionControll::StaticClass(), FoundActors);

		if (FoundActors.Num() == 0)
		{
			BroadcastScript("No Map Version Controller. Update Map From GoogleDrive.");
		}
		else
		{
			for (AActor* Actor : FoundActors)
			{
				if (ALSMapVersionControll* MapVersion = Cast<ALSMapVersionControll>(Actor))
				{
					int32 CurrentMapVersion = MapVersion->GetCurrentMapVersion();
					if (CurrentMapVersion != MapVersions[MapName])
					{
						BroadcastScript("Not Match Map Version. Update Map From GoogleDrive.");
					}
				}
			}
		}
	}
}


void ALSGameMode::OnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
{
	bIsSiJaeDragging = InIsSiJaeDragging;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
		{
			if (PC->IsLocalController())
			{
				PC->CalledOnChangeSiJaeDragState(InIsSiJaeDragging);
			}
			else
			{
				PC->ClientRPCCalledOnChangeSiJaeDragState(InIsSiJaeDragging);
			}
		}
	}
}

void ALSGameMode::Start2DPuzzle(float Timer, const FName& InWidgetName)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
		{
			PC->MulticastRPCStart2DPuzzle(InWidgetName);
		}
	}

	if (Timer > 0) StartPuzzleTimer(Timer);
}

void ALSGameMode::End2DPuzzle()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
		{
			PC->MulticastRPCEnd2DPuzzle();
		}
	}

	EndPuzzleTimer();
}

void ALSGameMode::OnClear2DPuzzle()
{
	QuestComplete();
	End2DPuzzle();
	On2DPuzzleClear.Broadcast();
}

void ALSGameMode::OnFailed2DPuzzle()
{
	End2DPuzzle();
	On2DPuzzleFailed.Broadcast();
}

void ALSGameMode::StartPuzzleTimer(float InPuzzleTimerCount)
{
	if (GetWorldTimerManager().IsTimerActive(PuzzleTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(PuzzleTimerHandle);
	}

	CurrentPuzzleTime = InPuzzleTimerCount;
	GetWorldTimerManager().SetTimer(PuzzleTimerHandle, this, &ALSGameMode::SetPuzzleTimer, 1.0f, true);
}

void ALSGameMode::SetPuzzleTimer()
{
	CurrentPuzzleTime--;

	if (CurrentPuzzleTime < KINDA_SMALL_NUMBER)
	{
		OnFailed2DPuzzle();
	}
	else
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
			{
				PC->MulticastRPCUpdate2DPuzzleTimer(CurrentPuzzleTime);
			}
		}
	}
}

void ALSGameMode::EndPuzzleTimer()
{
	if (GetWorldTimerManager().IsTimerActive(PuzzleTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(PuzzleTimerHandle);
	}
}

