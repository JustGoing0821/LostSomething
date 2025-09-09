// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerController.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "UserInterface/LSQuestWidget.h"
#include "Net/UnrealNetwork.h"
#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSSijaeCursorPosInterface.h"
#include "Interface/LS2DPuzzleGameModeInterface.h"
#include "Interface/LSStartGameInterface.h"


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

	//Death
	static ConstructorHelpers::FClassFinder<ULSDeathWidget> DeathWidgetRef(TEXT("/Game/Players/UI/WBP_DeathWidget.WBP_DeathWidget_C"));
	if (DeathWidgetRef.Class)
	{
		DeathWidgetClass = DeathWidgetRef.Class;
	}

	//Blood
	static ConstructorHelpers::FClassFinder<UBloodWidget> BloodWidgetRef(TEXT("/Game/Players/UI/WBP_Blood.WBP_Blood_C"));
	if (BloodWidgetRef.Class)
	{
		BloodWidgetClass = BloodWidgetRef.Class;
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

	bIs2DPuzzleActive = false;
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

	if (IsLocalController() && BloodWidgetClass)
	{
		BloodWidget = CreateWidget<UBloodWidget>(this, BloodWidgetClass);
		if (BloodWidget)
		{
		
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("BloodhWidget Created."));
		}
	}


	if (IsLocalController() && DeathWidgetClass)
	{
		DeathWidget = CreateWidget<ULSDeathWidget>(this, DeathWidgetClass);
		if (DeathWidget)
		{
			// 생성만 하고 화면에는 추가x(숨김 상태)
			//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("DeathWidget Created."));
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

	SetInputMode(FInputModeGameOnly());


	if (IsLocalController() && !HasAuthority())
	{
		ServerRPCStartGame();
	}
}

void ALSPlayerController::Tick(float DeltaTime)
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

void ALSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSPlayerController, CharacterChoice);
	DOREPLIFETIME(ALSPlayerController, SiJaeCursorPos);
}

//Blood
void ALSPlayerController::ShowBloodWidget()
{
	
	MulticastShowBloodWidget();
}

void ALSPlayerController::MulticastShowBloodWidget_Implementation()
{

	if (IsLocalController() && BloodWidget)
	{
		BloodWidget->AddToViewport();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ALSPlayerController::RemoveBloodWidget, 1.0f);

	}

	DisableInput(this);

}

void ALSPlayerController::RemoveBloodWidget()
{
	if (BloodWidget && BloodWidget->IsInViewport())
	{
		BloodWidget->RemoveFromParent();
	}

	EnableInput(this);
}


//Death
void ALSPlayerController::ShowDeathWidget()
{
	if (IsLocalController() && DeathWidget)
	{
		DeathWidget->AddToViewport();
		//UE_LOG(LogTemp, Warning, TEXT("Death widget shown"));
	}
}

void ALSPlayerController::HideDeathWidget()
{
	if (IsLocalController() && DeathWidget)
	{
		DeathWidget->RemoveFromParent();
		//UE_LOG(LogTemp, Warning, TEXT("Death widget hidden"));
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

void ALSPlayerController::StartTalking()
{
	UE_LOG(LogTemp, Log, TEXT("Voice started"));

}

void ALSPlayerController::StopTalking()
{
	UE_LOG(LogTemp, Log, TEXT("Voice stopped"));
}

void ALSPlayerController::Start2DPuzzle(const FName& InWidgetName)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	bIs2DPuzzleActive = true;

	if (IsLocalController() && LS2DPuzzleHUDClass)
	{
		StopKeyInput();
		SetInputMode(FInputModeUIOnly());
		SetShowMouseCursor(true);

		LS2DPuzzleHUDWidget = CreateWidget<ULS2DPuzzleHUD>(this, LS2DPuzzleHUDClass);
		if (LS2DPuzzleHUDWidget)
		{
			LS2DPuzzleHUDWidget->AddToViewport(0);

			LS2DPuzzleHUDWidget->OnStartWidget(InWidgetName);
		}
	}
}

void ALSPlayerController::End2DPuzzle()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
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

void ALSPlayerController::OnExit2DPuzzle(uint8 InIsExitTogether)
{
	if (InIsExitTogether)
	{
		if (HasAuthority())
		{
			ILS2DPuzzleGameModeInterface* PuzzleInterface = Cast<ILS2DPuzzleGameModeInterface>(GetWorld()->GetAuthGameMode());
			if (PuzzleInterface)
			{
				PuzzleInterface->End2DPuzzle();
			}
		}
		else
		{
			ServerRPCOnExit2DPuzzle();
		}
	}
	else
	{
		End2DPuzzle();
	}
}

void ALSPlayerController::OnClear2DPuzzle()
{
	if (HasAuthority())
	{
		ILS2DPuzzleGameModeInterface* GameMode = Cast<ILS2DPuzzleGameModeInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->OnClear2DPuzzle();
		}
	}
	else
	{
		ServerRPCOnClear2DPuzzle();
	}
}

void ALSPlayerController::UpdateAim(const FString& InString)
{
	LSHUDWidget->UpdateAim(InString);
}

void ALSPlayerController::Update2DPuzzleTimer(float Timer)
{
	LS2DPuzzleHUDWidget->UpdateTimer(Timer);
}

void ALSPlayerController::OnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
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

void ALSPlayerController::CalledOnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
{
	//LS_LOG(LogLS, Log, TEXT("Begin : %d"), InIsSiJaeDragging);
	LS2DPuzzleHUDWidget->SetbIsSiJaeDragging(InIsSiJaeDragging);
}

void ALSPlayerController::GetSiJaeLocalCursor()
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

void ALSPlayerController::SetGameModeSiJaeCursor(const FVector2D& InSiJaeCursorPos)
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	ILSSijaeCursorPosInterface* GameModeCursor = Cast<ILSSijaeCursorPosInterface>(GameMode);
	if (GameModeCursor)
	{
		GameModeCursor->SetSiJaeCursorPos(InSiJaeCursorPos);
	}
}

void ALSPlayerController::SendOnChangeSiJaeDragState(uint8 InIsSiJaeDragging)
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

void ALSPlayerController::StopKeyInput()
{
	if (HasAuthority())
	{
		if (IsLocalController())
		{
			if (PlayerInput) PlayerInput->FlushPressedKeys();
		}
		else
		{
			ClientRPCStopKeyInput();
			if (GetPawn())
			{
				GetPawn()->GetMovementComponent()->StopMovementImmediately();
			}
		}
	}
	else
	{
		if (IsLocalController())
		{
			if (PlayerInput) PlayerInput->FlushPressedKeys();
			ServerRPCStopMovement();
		}
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

void ALSPlayerController::ServerRPCSetGameModeSiJaeCursor_Implementation(const FVector2D& InSiJaeCursorPos)
{
	SetGameModeSiJaeCursor(InSiJaeCursorPos);
}

void ALSPlayerController::ServerRPCSendOnChangeSiJaeDragState_Implementation(uint8 InIsSiJaeDragging)
{
	//LS_LOG(LogLS, Log, TEXT("Begin : %d"), InIsSiJaeDragging);
	SendOnChangeSiJaeDragState(InIsSiJaeDragging);
}

void ALSPlayerController::ClientRPCCalledOnChangeSiJaeDragState_Implementation(uint8 InIsSiJaeDragging)
{
	CalledOnChangeSiJaeDragState(InIsSiJaeDragging);
}

void ALSPlayerController::MulticastRPCStart2DPuzzle_Implementation(const FName& InWidgetName)
{
	Start2DPuzzle(InWidgetName);
}

void ALSPlayerController::MulticastRPCEnd2DPuzzle_Implementation()
{
	End2DPuzzle();
}

void ALSPlayerController::ServerRPCOnExit2DPuzzle_Implementation()
{
	if (HasAuthority())
	{
		ILS2DPuzzleGameModeInterface* PuzzleInterface = Cast<ILS2DPuzzleGameModeInterface>(GetWorld()->GetAuthGameMode());
		if (PuzzleInterface)
		{
			PuzzleInterface->End2DPuzzle();
		}
	}
}

void ALSPlayerController::ServerRPCOnClear2DPuzzle_Implementation()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		ILS2DPuzzleGameModeInterface* GameMode = Cast<ILS2DPuzzleGameModeInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->OnClear2DPuzzle();
		}
	}
}

void ALSPlayerController::MulticastRPCUpdate2DPuzzleTimer_Implementation(float Timer)
{
	if (IsLocalController())
	{
		Update2DPuzzleTimer(Timer);
	}
}

void ALSPlayerController::ServerRPCStartGame_Implementation()
{
	ILSStartGameInterface* GameMode = Cast<ILSStartGameInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->StartGame();
	}
}

void ALSPlayerController::ServerRPCStopMovement_Implementation()
{
	if (!IsLocalController())
	{
		if (GetPawn())
		{
			GetPawn()->GetMovementComponent()->StopMovementImmediately();
		}
	}
}

void ALSPlayerController::ClientRPCStopKeyInput_Implementation()
{
	if (PlayerInput) PlayerInput->FlushPressedKeys();
}
