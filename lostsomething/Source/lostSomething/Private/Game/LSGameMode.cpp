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
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"


ALSGameMode::ALSGameMode()
{
	bUseSeamlessTravel = true;

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

	static ConstructorHelpers::FClassFinder<APawn> VRPawnClassRef(TEXT("/Game/VR/VRTemplate/Blueprints/VRPawn.VRPawn_C"));
	if (VRPawnClassRef.Class != NULL)
	{
		VRPawnClass = VRPawnClassRef.Class;
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

	//2D Section
	bIsSiJaeDragging = false;
	isSeverPossesed = false;
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

	SetDefaultPawn(ResultController);

	/*
	//Used ChooseCharacterMap
	if (GameInstance)
	{
		ALSPlayerController* LSPlayerController = Cast<ALSPlayerController>(ResultController);
		if (LSPlayerController)
		{
			if (GameInstance->isVR != true)
			{
				//VR 아닐때
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
			}
			else
			{
				//VR 일때
				if (LSPlayerController->GetName() == TEXT("LSPlayerController_0"))
				{
					ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
					LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
					DefaultPawnClass = SiJaePawnClass;

					CurrentPlayerCount++;

					FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
				}
				else
				{
					ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
					LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
					DefaultPawnClass = VRPawnClass;

					CurrentPlayerCount++;

					FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
				}
			}

			//Quest Widget Update Bind
			QuestManager->OnQuestClearWidget.AddUObject(LSPlayerController, &ALSPlayerController::CallQuestClear);
			QuestManager->OnQuestStartWidget.AddUObject(LSPlayerController, &ALSPlayerController::CallQuestStart);
			QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALSPlayerController::UpdateQuestWidget);
		}
	}
	*/

	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
	return ResultController;
}

void ALSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//LS_LOG(LogLS, Log, TEXT("Begin"));
}

void ALSGameMode::StartGame(bool bisNeedQuestComplete)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//FTimerHandle Handle;
	//GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	//	{
	//		//Quest Start
	//		if (CurrentPlayerCount == 2)
	//		{
	//			QuestStart();
	//		}
	//	}
	//), 2.f, false);

	if (bisNeedQuestComplete)
	{
		QuestComplete();
	}
	else
	{
		QuestStart();
	}

	OnStartGame.Broadcast();
}

void ALSGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ALSGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("Begin"));

	if (C && C->PlayerState)
	{
		LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("C is exist!"));
		if (APawn* OldPawn = C->GetPawn())
		{
			LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("Destroy Pawn"));
			OldPawn->Destroy();
		}

		APlayerController* PC = Cast<APlayerController>(C);
		SetDefaultPawn(PC);

		// DefaultPawnClass 설정 확인
		LS_LOG(LogLSls, Warning, TEXT("DefaultPawnClass : %s"), DefaultPawnClass ? *DefaultPawnClass->GetName() : TEXT("NONE"));


		LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("RestartPlayer"));
		RestartPlayer(C);

		APawn* NewPawn = C->GetPawn();
		LS_LOG(LogLSls, Warning, TEXT("After RestartPlayer - Pawn : %s"), NewPawn ? *NewPawn->GetName() : TEXT("NONE"));
	}

	LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("End"));
}

void ALSGameMode::SetDefaultPawn(APlayerController*& InController)
{
	ULSGameInstance* GameInstance = Cast<ULSGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		ALSPlayerController* LSPlayerController = Cast<ALSPlayerController>(InController);
		if (LSPlayerController)
		{
			if (GameInstance->isVR != true)
			{
				LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("CharacterPossess Start!"));
				//VR 아닐때
				if (!isSeverPossesed)
				{
					if (GameInstance->GetServerCharacterChoice() == ELSCharacterChoice::SiJae)
					{
						ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
						LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
						DefaultPawnClass = SiJaePawnClass;
						LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("DefaultPawnClass Setted!"));
					}
					else
					{
						ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
						LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
						DefaultPawnClass = IJaePawnClass;
						LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("DefaultPawnClass Setted!"));
					}
					CurrentPlayerCount++;
					isSeverPossesed = true;

					FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
					LS_LOG(LogLSls, Error, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
				}
				else
				{
					if (GameInstance->GetClientCharacterChoice() == ELSCharacterChoice::IJae)
					{
						ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
						LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
						DefaultPawnClass = IJaePawnClass;
						LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("DefaultPawnClass Setted!"));
					}
					else
					{
						ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
						LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
						DefaultPawnClass = SiJaePawnClass;
						LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("DefaultPawnClass Setted!"));
					}
					CurrentPlayerCount++;
					isSeverPossesed = false;

					FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
					LS_LOG(LogLS, Error, TEXT("%s Setting %s"), *LSPlayerController->GetName(), *EnumString);
				}
			}
			else
			{
				//VR 일때
				if (!isSeverPossesed)
				{
					ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
					LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::SiJae);
					DefaultPawnClass = SiJaePawnClass;

					CurrentPlayerCount++;
					isSeverPossesed = true;

					FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
				}
				else
				{
					ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
					LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
					DefaultPawnClass = VRPawnClass;

					CurrentPlayerCount++;
					isSeverPossesed = false;

					FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayerController->GetCharacterChoice())).ToString();
				}
			}

			//Quest Widget Update Bind
			QuestManager->OnQuestClearWidget.AddUObject(LSPlayerController, &ALSPlayerController::CallQuestClear);
			QuestManager->OnQuestStartWidget.AddUObject(LSPlayerController, &ALSPlayerController::CallQuestStart);
			QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALSPlayerController::UpdateQuestWidget);
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("ALSPlayerController Not Found!!"));
		}
	}
	else
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("GameInstance Not Found!!"));
	}

	LS_LOG(LogLSls, Warning, TEXT("%s"), TEXT("End"));
}

bool ALSGameMode::IsVRPlayer(APlayerController* Controller) const
{
	// GEngine이 유효하고, XRSystem(즉, VR/AR 추적 시스템)이 존재하는지 확인
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		// 현재 헤드 트래킹이 허용되어 있다면 VR 장치가 연결된 상태임
		return GEngine->XRSystem->IsHeadTrackingAllowed();
	}

	// XR 시스템이 없으면 VR 아님
	return false;
}

void ALSGameMode::QuestStart()
{
	QuestManager->QuestStart();
}

void ALSGameMode::QuestComplete()
{
	QuestManager->QuestComplete();
}

void ALSGameMode::MenuOption(FString Option)
{
	UWorld* World = GetWorld();
	if (!World)
		return;
	if (Option == "ChooseMap")
	{
		bool bResult = World->ServerTravel(TEXT("/Game/Map/LevelChooseMap?listen"), true, false);

		if (!bResult)
		{
			UE_LOG(LogTemp, Error, TEXT("ServerTravel Failed!"));
		}
	}
	else if(Option == "Lobby"){
		bool bResult = World->ServerTravel(TEXT("/Game/Map/LobbyMap?listen"), true, false);

		if (!bResult)
		{
			UE_LOG(LogTemp, Error, TEXT("ServerTravel Failed!"));
		}
	}
	
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
	if (!LSPlayerController) return;

	// 1. VR 기기를 사용 중인 플레이어인 경우
	if (IsVRPlayer(LSPlayerController))
	{
		ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LSPlayerController);
		if (LSCharacterChoice)
		{
			// VR 전용 캐릭터로 설정
			LSCharacterChoice->SetCharacterChoice(ELSCharacterChoice::IJae);
			DefaultPawnClass = VRPawnClass;
			CurrentPlayerCount++;
		}
	}
	else
	{
		// 2. 일반 PC 플레이어의 경우 (기존 로직 그대로)
		const bool bIsFirstPlayer = (LSPlayerController->GetName() == TEXT("LSPlayerController_0"));

		if (bIsFirstPlayer)
		{
			if (bIsSiJaeServer)
			{
				Cast<ILSCharacterChoiceInterface>(LSPlayerController)->SetCharacterChoice(ELSCharacterChoice::SiJae);
				DefaultPawnClass = SiJaePawnClass;
			}
			else
			{
				Cast<ILSCharacterChoiceInterface>(LSPlayerController)->SetCharacterChoice(ELSCharacterChoice::IJae);
				DefaultPawnClass = IJaePawnClass;
			}
		}
		else
		{
			if (bIsSiJaeServer)
			{
				Cast<ILSCharacterChoiceInterface>(LSPlayerController)->SetCharacterChoice(ELSCharacterChoice::IJae);
				DefaultPawnClass = IJaePawnClass;
			}
			else
			{
				Cast<ILSCharacterChoiceInterface>(LSPlayerController)->SetCharacterChoice(ELSCharacterChoice::SiJae);
				DefaultPawnClass = SiJaePawnClass;
			}
		}
		CurrentPlayerCount++;
	}

	// 3. 퀘스트 위젯 업데이트 함수 바인딩
	QuestManager->OnQuestClearWidget.AddUObject(LSPlayerController, &ALSPlayerController::CallQuestClear);
	QuestManager->OnQuestStartWidget.AddUObject(LSPlayerController, &ALSPlayerController::CallQuestStart);
	QuestManager->OnQuestStart.AddUObject(LSPlayerController, &ALSPlayerController::UpdateQuestWidget);
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

void ALSGameMode::Start2DPuzzle(const FName& InWidgetName, const FVector2D& InGoalPos)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(Iterator->Get()))
		{
			PC->MulticastRPCStart2DPuzzle(InWidgetName, InGoalPos);
		}
	}
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

	//EndPuzzleTimer();
}

void ALSGameMode::OnClear2DPuzzle()
{
	QuestComplete();
	End2DPuzzle();
	EndPuzzleTimer();
	On2DPuzzleClear.Broadcast();
}

void ALSGameMode::OnFailed2DPuzzle()
{
	End2DPuzzle();
	EndPuzzleTimer();
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

	//LS_LOG(LogLSls, Log, TEXT("CurrentPuzzleTime : %f"), CurrentPuzzleTime);
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

	//LS_LOG(LogLSls, Log, TEXT("CurrentPuzzleTime : %f"), CurrentPuzzleTime);
}

void ALSGameMode::EndPuzzleTimer()
{
	if (GetWorldTimerManager().IsTimerActive(PuzzleTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(PuzzleTimerHandle);
	}
}

//chat
void ALSGameMode::BroadcastChatMessage(const FString& Sender, const FString& Text)
{

	//메시지 전달
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ALSPlayerController* PC = Cast<ALSPlayerController>(It->Get()))
		{
			PC->ClientReceiveChatMessage(Sender, Text);
		}
	}
}

// Sequence Skip
void ALSGameMode::StartSequenceMode()
{
	bIsSequencePlaying = true;
	PlayerSkipStates.Empty();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			PlayerSkipStates.Add(PC, false);
		}
	}
}

void ALSGameMode::OnPlayerSkipSequence(APlayerController* PC)
{
	if (!bIsSequencePlaying) return;

	if (PlayerSkipStates.Contains(PC))
	{
		PlayerSkipStates[PC] = true;

		UE_LOG(LogTemp, Log, TEXT("Player %s skipped sequence"), *PC->GetName());

		if (AreAllPlayersSkipped())
		{
			UE_LOG(LogTemp, Warning, TEXT("All players skipped! Ending sequence..."));
			ForceEndSequence();
		}
		else
		{
			ALSPlayerController* LSPC = Cast<ALSPlayerController>(PC);
			if (LSPC)
			{
				LSPC->ClientRPCShowWaitingForOtherPlayer();
			}
		}
	}
}

bool ALSGameMode::AreAllPlayersSkipped() const
{
	for (const auto& Pair : PlayerSkipStates)
	{
		if (!Pair.Value)
		{
			return false;
		}
	}
	return true;
}

void ALSGameMode::ForceEndSequence()
{
	bIsSequencePlaying = false;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALSPlayerController* LSPC = Cast<ALSPlayerController>(It->Get());
		if (LSPC)
		{
			LSPC->ClientRPCForceEndSequence();
		}
	}
}