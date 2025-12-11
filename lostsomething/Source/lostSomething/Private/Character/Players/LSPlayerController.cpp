// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerController.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/Players/LSPlayer.h"
#include "Character/UI/LSChatWidget.h"
#include "Game/LSGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSHpWidget.h"
#include "Character/UI/LSChatWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Character/UI/BloodWidget.h"
#include "Character/UI/LSDeathWidget.h" 
#include "UserInterface/LSQuestWidget.h"
#include "Net/UnrealNetwork.h"
#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSSijaeCursorPosInterface.h"
#include "Interface/LS2DPuzzleGameModeInterface.h"
#include "Interface/LSStartGameInterface.h"
#include "BossNPC/BMSpawner.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "UserInterface/MiniMap/MiniMapWidget.h"
#include "UserInterface/MenuWidget.h"
#include "Runtime/MediaAssets/Public/FileMediaSource.h"


ALSPlayerController::ALSPlayerController()
{
	//HUD
	static ConstructorHelpers::FClassFinder<ULSHUDWidget> LSHUDWidgetRef(TEXT("/Game/Players/UI/WBP_LSHUD.WBP_LSHUD_C"));
	if (LSHUDWidgetRef.Class)
	{
		LSHUDWidgetClass = LSHUDWidgetRef.Class;
	}

	//Hp
	static ConstructorHelpers::FClassFinder<ULSHpWidget> LSHpWidgetRef(TEXT("/Game/Players/UI/WBP_Hp.WBP_Hp_C"));
	if (LSHpWidgetRef.Class)
	{
		LSHpWidgetClass = LSHpWidgetRef.Class;
	}

	//chat
	static ConstructorHelpers::FClassFinder<ULSChatWidget> ChatBP(
		TEXT("/Game/Players/UI/WBP_Chat.WBP_Chat_C"));
	if (ChatBP.Succeeded())
	{
		ChatWidgetClass = ChatBP.Class; // 이후 BeginPlay에서 이 클래스로 생성
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

	//MiniMap Section
	static ConstructorHelpers::FClassFinder<UMiniMapWidget> MiniMapWidgetRef(TEXT("/Game/UI/MiniMap/WBP_MiniMapWidget.WBP_MiniMapWidget_C"));
	if (MiniMapWidgetRef.Class)
	{
		MiniMapWidgetClass = MiniMapWidgetRef.Class;
	}
	bIsMinimap = false;

	//Menu Section
	static ConstructorHelpers::FClassFinder<UMenuWidget> MenuWidgetRef(TEXT("/Game/UI/Menu/WBP_Menu.WBP_Menu_C"));
	if (MenuWidgetRef.Class)
	{
		MenuWidgetClass = MenuWidgetRef.Class;
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

	static ConstructorHelpers::FClassFinder<UUserWidget> MediaPlayerWidgetRef(TEXT("/Game/Sequences/Sequencers/SequencePlayers/WBP_LSMediaPlay.WBP_LSMediaPlay_C"));
	if (MediaPlayerWidgetRef.Class)
	{
		MediaPlayerWidgetClass = MediaPlayerWidgetRef.Class;
	}

	bIs2DPuzzleActive = false;
}

void ALSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
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


	if (IsLocalController() && LSHpWidgetClass)
	{
		LSHpWidget = CreateWidget<ULSHpWidget>(this, LSHpWidgetClass);
		if (LSHpWidget)
		{
			LSHpWidget->AddToViewport();
		}
	}

	//chat
	if (IsLocalController() && ChatWidgetClass)
	{
		ChatWidget = CreateWidget<ULSChatWidget>(this, ChatWidgetClass);
		if (ChatWidget)
		{
			ChatWidget->AddToViewport(5);
			ChatWidget->SetVisibility(ESlateVisibility::Collapsed);

			ChatWidget->OnChatCommitted = ULSChatWidget::FOnChatCommitted::CreateLambda(
				[this](const FString& Msg)
				{
					const FString Clean = Msg.TrimStartAndEnd();
					if (!Clean.IsEmpty())
					{
						ServerSendChatMessage(Clean.Left(200));
					}
				});
		}
	}

	//minimap
	CreateMinimapWidget();


	//Menu
	if (IsLocalController() && MenuWidgetClass)
	{
		MenuWidget = CreateWidget<UMenuWidget>(this, MenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
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
	//if (IsLocalController() && !HasAuthority())
	//{
	//	ServerRPCStartGame();
	//}

	if (IsLocalController())
	{
		if (LSHUDWidget)	 LSHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (LSHpWidget) LSHpWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (QuestWidget) QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (MiniMapWidget) MiniMapWidget->SetVisibility(ESlateVisibility::Collapsed);
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

void ALSPlayerController::CallQuestClear()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	
	if (IsLocalController())
	{
		QuestWidget->CallQuestClear();
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
	else
	{
		ClientRPCCallQuestClear();
		//LS_LOG(LogLS, Log, TEXT("%s ClientRPCUpdateQuestWidget called"), *EnumString);
	}
}

void ALSPlayerController::CallQuestStart(FLSQuestData InQuestData)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	if (IsLocalController())
	{
		QuestWidget->CallQuestStart(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
	else
	{
		ClientRPCCallQuestStart(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s ClientRPCUpdateQuestWidget called"), *EnumString);
	}
}

void ALSPlayerController::UpdateQuestWidget(FLSQuestData InQuestData, ELSInteractionEnum InInteractionEnum)
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s Begin"), *EnumString);

	if (IsLocalController())
	{
		if(QuestWidget) QuestWidget->UpdateQuestWidget(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
	else
	{
		ClientRPCUpdateQuestWidget(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s ClientRPCUpdateQuestWidget called"), *EnumString);
	}

	if (InQuestData.CurrentQuestEnum == ELSInteractionEnum::Quest7)
	{
		if (IsLocalController())
			RemoveMinimapWidget();
		else
			ClientMinimapWidget();
	}
	else if (InQuestData.CurrentQuestEnum == ELSInteractionEnum::Quest9)
	{
		if (IsLocalController())
			CreateMinimapWidget();
		else
			ClientMinimapWidget();
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

void ALSPlayerController::Start2DPuzzle(const FName& InWidgetName, const FVector2D& InGoalPos)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//LS_LOG(LogLSls, Log, TEXT("InGoalPos : %f, %f"), InGoalPos.X, InGoalPos.Y);

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

			LS2DPuzzleHUDWidget->OnStartWidget(InWidgetName, InGoalPos);
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
	if(LSHUDWidget) LSHUDWidget->UpdateAim(InString);
}

void ALSPlayerController::Update2DPuzzleTimer(float Timer)
{
	if(LS2DPuzzleHUDWidget) LS2DPuzzleHUDWidget->UpdateTimer(Timer);
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
	if(LS2DPuzzleHUDWidget) LS2DPuzzleHUDWidget->SetbIsSiJaeDragging(InIsSiJaeDragging);
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

void ALSPlayerController::StartSequence(bool InIsMapStart, bool InNeedQuestComplete, UFileMediaSource* InVideoSource, class USoundBase* InSoundSource)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	StopKeyInput();
	SetInputMode(FInputModeUIOnly());


	ALSPlayer* LSPlayer = Cast<ALSPlayer>(GetPawn());
	if (LSPlayer) LSPlayer->StartSequence();


	if (IsLocalController())
	{
		if (LSHUDWidget)	 LSHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (LSHpWidget) LSHpWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (QuestWidget) QuestWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (MiniMapWidget) MiniMapWidget->SetVisibility(ESlateVisibility::Collapsed);

		StopBGM();

		if (MediaPlayerWidgetClass)
		{
			UUserWidget* MediaPlayerWidget = CreateWidget<UUserWidget>(this, MediaPlayerWidgetClass);
			if (MediaPlayerWidget)
			{
				UFunction* Func = MediaPlayerWidget->FindFunction(FName("SetMediaParams"));
				if (Func)
				{
					struct { 
						bool bIsMapStart;
						bool bisNeedQuestComplete;
						UFileMediaSource* VideoSource;
						USoundBase* SoundSource;
					} Params;

					Params.bIsMapStart = InIsMapStart;
					Params.bisNeedQuestComplete = InNeedQuestComplete;
					Params.VideoSource = InVideoSource;
					Params.SoundSource = InSoundSource;
					MediaPlayerWidget->ProcessEvent(Func, &Params);
				}

				MediaPlayerWidget->AddToViewport();
			}
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No MediaPlayerWidgetClass!!"));
		}
	}
	else
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("Is Not Local Controller"));
	}
}

void ALSPlayerController::EndSequence(bool bIsMapStart, bool bisNeedQuestComplete)
{
	//LS_LOG(LogLSls, Log, TEXT("Begin : % d"), IsLocalController());

	SetInputMode(FInputModeGameOnly());

	ALSPlayer* LSPlayer = Cast<ALSPlayer>(GetPawn());
	if (LSPlayer) LSPlayer->EndSequence();

	if (IsLocalController())
	{
		if (LSHUDWidget)	 LSHUDWidget->SetVisibility(ESlateVisibility::Visible);
		if (LSHpWidget) LSHpWidget->SetVisibility(ESlateVisibility::Visible);
		if (QuestWidget) QuestWidget->SetVisibility(ESlateVisibility::Visible);
		if (MiniMapWidget) MiniMapWidget->SetVisibility(ESlateVisibility::Visible);

		StartBGM();
	}

	if (bIsMapStart && IsLocalController() && !HasAuthority())
	{
		ServerRPCStartGame(bisNeedQuestComplete);
	}
}

void ALSPlayerController::StartBGM()
{
	OnBGMStart.Broadcast();
}

void ALSPlayerController::StopBGM()
{
	OnBGMStop.Broadcast();
}

void ALSPlayerController::ClientRPCUpdateQuestWidget_Implementation(FLSQuestData InQuestData)
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (this->IsLocalController())
	{
		if(QuestWidget) QuestWidget->UpdateQuestWidget(InQuestData);
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

void ALSPlayerController::MulticastRPCStart2DPuzzle_Implementation(const FName& InWidgetName, const FVector2D& InGoalPos)
{
	Start2DPuzzle(InWidgetName, InGoalPos);
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

void ALSPlayerController::ServerRPCStartGame_Implementation(bool bisNeedQuestComplete)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	ILSStartGameInterface* GameMode = Cast<ILSStartGameInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->StartGame(bisNeedQuestComplete);
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

//Minimap

void ALSPlayerController::CreateMinimapWidget()
{
	//미니맵 위젯 생성

	if (IsLocalController() && MiniMapWidgetClass)
	{
		MiniMapWidget = CreateWidget<UMiniMapWidget>(this, MiniMapWidgetClass);
		if (MiniMapWidget && !bIsMinimap)
		{
			MiniMapWidget->AddToViewport();
			bIsMinimap = true;
		}
	}
}

void ALSPlayerController::RemoveMinimapWidget()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (MiniMapWidget && bIsMinimap)
	{
		MiniMapWidget->RemoveFromParent();
		bIsMinimap = false;
	}		
}

void ALSPlayerController::ClientMinimapWidget_Implementation()
{
	if (this->IsLocalController())
	{	
		if (bIsMinimap)
			RemoveMinimapWidget();
		else
			CreateMinimapWidget();
	}
}


// Menu

void ALSPlayerController::OpenMenu()
{
	if(MenuWidget) MenuWidget->MenuVisibility();
}

void ALSPlayerController::MenuToLevel(const FString& Option)
{
	ServerMenuToLevel(Option);
}

void ALSPlayerController::ServerMenuToLevel_Implementation(const FString& Option)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// GameMode는 서버에서만 유효하므로 여기서 안전하게 접근 가능
	ALSGameMode* GM = Cast<ALSGameMode>(UGameplayStatics::GetGameMode(World));
	if (GM)
	{
		// 이제 GameMode의 로직을 사용할 수 있습니다.
		GM->MenuOption(Option);
	}
}


//chat
void ALSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//엔터로 열고 ESC로 닫음
	InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ALSPlayerController::OpenChat);
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ALSPlayerController::CloseChat);

}

void ALSPlayerController::OpenChat()
{
	if (!ChatWidget) return;

	ChatWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetUIOnlyInput();   
	ChatWidget->FocusInput();
}

void ALSPlayerController::CloseChat()
{
	if (!ChatWidget) return;

	ChatWidget->SetVisibility(ESlateVisibility::Collapsed);
	SetGameOnlyInput();  
}

void ALSPlayerController::SetUIOnlyInput()
{
	FInputModeUIOnly M;
	M.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(M);
	SetShowMouseCursor(true);
}

void ALSPlayerController::SetGameOnlyInput()
{
	FInputModeGameOnly M;
	SetInputMode(M);
	SetShowMouseCursor(false);
}



// 클라 → 서버 
void ALSPlayerController::ServerSendChatMessage_Implementation(const FString& Text)
{
	if (Text.TrimStartAndEnd().IsEmpty()) return;

	// 서버 권한 확인
	UWorld* World = GetWorld();
	if (!World) return;

	// 스팸/홍수 방지(서버 시계 기준 쿨다운)
	const float Now = World->GetTimeSeconds();
	const float Cooldown = 0.35f; 
	if (Now - LastChatTimeServer < Cooldown)
	{
		return;
	}
	LastChatTimeServer = Now;

	// 보낸 사람 이름
	const FString Sender = (PlayerState ? PlayerState->GetPlayerName() : TEXT("Player"));

	// GameMode를 통해 모든 PC에 브로드캐스트
	if (ALSGameMode* GM = World->GetAuthGameMode<ALSGameMode>())
	{
		// 필요 시 욕설 필터/길이제한/금지어 처리 등을 여기에서 추가
		GM->BroadcastChatMessage(Sender, Text);
	}
}

// UI에 표시
void ALSPlayerController::ClientReceiveChatMessage_Implementation(const FString& Sender, const FString& Text)
{
	if (ChatWidget)
	{
		ChatWidget->AddChatLine(Sender, Text);
	}
}

void ALSPlayerController::ClientRPCCallQuestClear_Implementation()
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (this->IsLocalController())
	{
		if(QuestWidget) QuestWidget->CallQuestClear();
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
}

void ALSPlayerController::ClientRPCCallQuestStart_Implementation(FLSQuestData InQuestData)
{
	FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoice)).ToString();
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	if (this->IsLocalController())
	{
		if (QuestWidget) QuestWidget->CallQuestStart(InQuestData);
		//LS_LOG(LogLS, Log, TEXT("%s UpdateQuestWidget Updated"), *EnumString);
	}
}

void ALSPlayerController::ClientRPCStartSequence_Implementation(bool InIsMapStart, bool InNeedQuestComplete, UFileMediaSource* InVideoSource, class USoundBase* InSoundSource)
{
	if (IsLocalController())
	{
		StartSequence(InIsMapStart, InNeedQuestComplete, InVideoSource, InSoundSource);
	}
}
