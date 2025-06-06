// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSHUDWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Net/UnrealNetwork.h"


ALSPlayerController::ALSPlayerController()
{
	static ConstructorHelpers::FClassFinder<ULSHUDWidget> LSHUDWidgetRef(TEXT("/Game/Players/UI/WBP_LSHUD.WBP_LSHUD_C"));
	if (LSHUDWidgetRef.Class)
	{
		LSHUDWidgetClass = LSHUDWidgetRef.Class;
	}


}

void ALSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);


	LSHUDWidget = CreateWidget<ULSHUDWidget>(this, LSHUDWidgetClass);
	if (LSHUDWidget)
	{
		LSHUDWidget->AddToViewport();
	}

	if (ScriptWidgetClass) 
	{
		ScriptWidget = CreateWidget<ULSScriptWidget>(this, ScriptWidgetClass);
		if (ScriptWidget) 
		{
			ScriptWidget->AddToViewport(10);
		}
	}
}

void ALSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSPlayerController, CharacterChoice);
}

ELSCharacterChoice ALSPlayerController::GetCharacterChoice()
{
	return CharacterChoice;
}

void ALSPlayerController::SetCharacterChoice(ELSCharacterChoice InCharacterChoice)
{
	CharacterChoice = InCharacterChoice;
}

void ALSPlayerController::ShowScript(const FString& ScriptText)
{
	if (ScriptWidget)
	{
		ScriptWidget->SetScriptText(ScriptText);
		ScriptWidget->ShowScriptWidget();
	}
}



void ALSPlayerController::SelectNextSlot()
{
	if (LSHUDWidget)
	{
		int32 CurrentSlot = LSHUDWidget->GetSelectedSlot();
		int32 NextSlot = CurrentSlot + 1;

		LSHUDWidget->ChangeSlot(NextSlot);

		UE_LOG(LogTemp, Warning, TEXT("Selected next slot: %d -> %d"), CurrentSlot, NextSlot);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LSHUDWidget is null in SelectNextSlot"));
	}
}

void ALSPlayerController::SelectPreviousSlot()
{
	if (LSHUDWidget)
	{
		int32 CurrentSlot = LSHUDWidget->GetSelectedSlot();
		int32 PreviousSlot = CurrentSlot - 1;

		LSHUDWidget->ChangeSlot(PreviousSlot);

		UE_LOG(LogTemp, Warning, TEXT("Selected previous slot: %d -> %d"), CurrentSlot, PreviousSlot);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LSHUDWidget is null in SelectPreviousSlot"));
	}
}

