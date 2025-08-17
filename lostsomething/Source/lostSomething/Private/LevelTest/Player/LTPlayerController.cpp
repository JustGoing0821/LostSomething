// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTPlayerController.h"
#include "lostSomething.h"
//#include "LevelTest/Game/LTGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Quest/LSQuestManager.h"
#include "Net/UnrealNetwork.h"
#include "UserInterface/LSQuestWidget.h"
#include "LevelTest/Player/LTScriptWidget.h"
#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "Interface/LSSijaeCursorPosInterface.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LS2DPuzzleInterface.h"


ALTPlayerController::ALTPlayerController()
{
	//Quest
	static ConstructorHelpers::FClassFinder<ULSQuestWidget> NEHUDWidgetRef(TEXT("/Game/UI/Quest/WBP_QuestWidget.WBP_QuestWidget_C"));
	if (NEHUDWidgetRef.Class)
	{
		QuestWidgetClass = NEHUDWidgetRef.Class;
	}

	static ConstructorHelpers::FClassFinder<ULTScriptWidget> ScriptWidgetRef(TEXT("/Game/Level/TestPlayer/BP_ScriptWidget.BP_ScriptWidget_C"));
	if (ScriptWidgetRef.Class)
	{
		ScriptWidgetClass = ScriptWidgetRef.Class;
	}


	//Puzzle Section
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 60.0f;

	static ConstructorHelpers::FClassFinder<ULS2DPuzzleHUD> LS2DPuzzleHUDRef(TEXT("/Game/Level/Puzzle/UI/Blueprints/WBP_2DPuzzleHUD.WBP_2DPuzzleHUD_C"));
	if (LS2DPuzzleHUDRef.Class)
	{
		LS2DPuzzleHUDClass = LS2DPuzzleHUDRef.Class;
	}

	//SetShowMouseCursor(true);
	bIs2DPuzzleActive = false;
}

void ALTPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
		ScriptWidget = CreateWidget<ULTScriptWidget>(this, ScriptWidgetClass);
		if (ScriptWidget)
		{
			ScriptWidget->AddToViewport();
			UpdateScriptWidget("");
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("ScriptWidget WidgetSetted."));
		}
	}

	SetInputMode(FInputModeGameOnly());
}

void ALTPlayerController::Tick(float DeltaTime)
{
	if (bIs2DPuzzleActive)
	{
		if (IsLocalPlayerController() && CharacterChoice == ELSCharacterChoice::SiJae)
		{
			GetSiJaeLocalCursor();
		}

		if (HasAuthority())
		{
			FVector2D SijaeCursorPosData;

			AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
			ILSSijaeCursorPosInterface* GameModeCursor = Cast<ILSSijaeCursorPosInterface>(GameMode);
			if (GameModeCursor)
			{
				SijaeCursorPosData = GameModeCursor->GetSiJaeCursorPos();
			}

			SiJaeCursorPos = SijaeCursorPosData;
		}

		if (IsLocalPlayerController())
		{
			if (LS2DPuzzleHUDWidget)
			{
				LS2DPuzzleHUDWidget->SetCursorPosition(SiJaeCursorPos);
				if (!HasAuthority())
				{
					//LS_LOG(LogLS, Log, TEXT("Test"));
					//LS_LOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorPos.X, SiJaeCursorPos.Y);
				}
			}
			else
			{
				//LS_LOG(LogLS, Error, TEXT("No LS2DPuzzleHUDWidget"));
			}
		}
	}
}

void ALTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALTPlayerController, CharacterChoice);
	DOREPLIFETIME(ALTPlayerController, SiJaeCursorPos);
	//DOREPLIFETIME(ALTPlayerController, bIs2DPuzzleActive);
}

void ALTPlayerController::UpdateScriptWidget(const FString& ScriptText)
{
	//LS_LOG(LogLS, Log, TEXT("Beggin - Received: %s"), *ScriptText);
	//LS_LOG(LogLS, Log, TEXT("Received length: %d"), ScriptText.Len());

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
	else if(HasAuthority())
	{
		ClientRPCUpdateScriptWidget(ScriptText);
	}
}

void ALTPlayerController::UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum)
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s Begin"), *EnumString);

	if (this->IsLocalController())
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

void ALTPlayerController::Start2DPuzzle(FName InPuzzleName, uint8 InIsStartTogether)
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	bIs2DPuzzleActive = true;

	if (IsLocalController() && LS2DPuzzleHUDClass)
	{
		SetInputMode(FInputModeUIOnly());
		if (CharacterChoice == ELSCharacterChoice::IJae)
		{
			SetShowMouseCursor(true);
		}


		LS2DPuzzleHUDWidget = CreateWidget<ULS2DPuzzleHUD>(this, LS2DPuzzleHUDClass);
		if (LS2DPuzzleHUDWidget)
		{
			LS2DPuzzleHUDWidget->AddToViewport(0);
		}
	}
}

void ALTPlayerController::End2DPuzzle(FName InPuzzleName, uint8 InIsEndTogether)
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	bIs2DPuzzleActive = false;

	if (IsLocalController() && LS2DPuzzleHUDClass)
	{
		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);

		if (LS2DPuzzleHUDWidget)
		{
			LS2DPuzzleHUDWidget->RemoveFromViewport();
		}
	}
}

void ALTPlayerController::OnBtnExitClicked()
{
	if (HasAuthority())
	{
		ILS2DPuzzleInterface* PuzzleInterface = Cast<ILS2DPuzzleInterface>(GetWorld()->GetAuthGameMode());
		if (PuzzleInterface)
		{
			PuzzleInterface->End2DPuzzle(TEXT("TestPuzzle"), true, this);
		}
	}
	else
	{
		ServerRPCOnBtnExitClicked();
	}
}

void ALTPlayerController::OnClear2DPuzzle()
{
	//if (HasAuthority())
	//{
	//	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//	if (HasAuthority())
	//	{
	//		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	//		if (GameModeQuest)
	//		{
	//			GameModeQuest->QuestComplete();
	//		}
	//	}
	//}
	//else
	//{
	//	ServerRPCOn2DPuzzleClear();
	//}

	if (HasAuthority())
	{
		On2DPuzzleClear.Broadcast();
	}
	else
	{
		ServerRPCOn2DPuzzleClear();
	}
}

FOn2DPuzzleClearDelegate& ALTPlayerController::Get2DPuzzleClearDelegate()
{
	return On2DPuzzleClear;
}

void ALTPlayerController::OnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
{
	if (HasAuthority())
	{
		SendOnChangeSiJaeDragState(InIsSiJaeDragging);
	}
	else
	{
		ServerRPCSendOnChangeSiJaeDragState(InIsSiJaeDragging);
	}
}

void ALTPlayerController::CalledOnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
{
	//LS_LOG(LogLS, Log, TEXT("Begin : %d"), InIsSiJaeDragging);
	LS2DPuzzleHUDWidget->SetbIsSiJaeDragging(InIsSiJaeDragging);
}

void ALTPlayerController::GetSiJaeLocalCursor()
{
	FVector2D CurPos;
	FVector2D MousePosition;
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetMousePosition(MousePosition);
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	CurPos = FVector2D(MousePosition.X / ViewportSize.X, MousePosition.Y / ViewportSize.Y);
	//LS_LOG(LogLS, Log, TEXT("Begin : %f, %f"), CurPos.X, CurPos.Y);

	if (HasAuthority())
	{
		SetGameModeSiJaeCursor(CurPos);
	}
	else
	{
		ServerRPCSetGameModeSiJaeCursor(CurPos);
	}
}

void ALTPlayerController::SetGameModeSiJaeCursor(const FVector2D& InSiJaeCursorPos)
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	ILSSijaeCursorPosInterface* GameModeCursor = Cast<ILSSijaeCursorPosInterface>(GameMode);
	if (GameModeCursor)
	{
		GameModeCursor->SetSiJaeCursorPos(InSiJaeCursorPos);
	}
}

void ALTPlayerController::SendOnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
{
	//LS_LOG(LogLS, Log, TEXT("Begin : %d"), InIsSiJaeDragging);
	if (HasAuthority())
	{
		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
		ILSSiJaeCursorDragInterface* GameModeCursor = Cast<ILSSiJaeCursorDragInterface>(GameMode);
		if (GameModeCursor)
		{
			GameModeCursor->OnChangeSiJaeDragState(InIsSiJaeDragging);
		}
	}
}

void ALTPlayerController::ClientRPCUpdateQuestWidget_Implementation(FLSQuestData InQuestData)
{
	//FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (this->IsLocalController())
	{
		QuestWidget->UpdateQuestWidget(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
}

void ALTPlayerController::ClientRPCUpdateScriptWidget_Implementation(const FString& ScriptText)
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

void ALTPlayerController::ServerRPCSetGameModeSiJaeCursor_Implementation(const FVector2D& InSiJaeCursorPos)
{
	SetGameModeSiJaeCursor(InSiJaeCursorPos);
}

void ALTPlayerController::ServerRPCSendOnChangeSiJaeDragState_Implementation(uint8 InIsSiJaeDragging)
{
	//LS_LOG(LogLS, Log, TEXT("Begin : %d"), InIsSiJaeDragging);
	SendOnChangeSiJaeDragState(InIsSiJaeDragging);
}

void ALTPlayerController::ClientRPCCalledOnChangeSiJaeDragState_Implementation(uint8 InIsSiJaeDragging)
{
	CalledOnChangeSiJaeDragState(InIsSiJaeDragging);
}

void ALTPlayerController::MulticastRPCStart2DPuzzle_Implementation(FName InPuzzleName, uint8 InIsStartTogether)
{
	Start2DPuzzle(InPuzzleName, InIsStartTogether);
}

void ALTPlayerController::MulticastRPCEnd2DPuzzle_Implementation(FName InPuzzleName, uint8 InIsEndTogether)
{
	End2DPuzzle(InPuzzleName, InIsEndTogether);
}

void ALTPlayerController::ServerRPCOnBtnExitClicked_Implementation()
{
	if (HasAuthority())
	{
		ILS2DPuzzleInterface* PuzzleInterface = Cast<ILS2DPuzzleInterface>(GetWorld()->GetAuthGameMode());
		if (PuzzleInterface)
		{
			PuzzleInterface->End2DPuzzle(TEXT("TestPuzzle"), true, this);
		}
	}
}

void ALTPlayerController::ServerRPCOn2DPuzzleClear_Implementation()
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//if (HasAuthority())
	//{
	//	ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	//	if (GameModeQuest)
	//	{
	//		GameModeQuest->QuestComplete();
	//	}
	//}
	On2DPuzzleClear.Broadcast();
}
