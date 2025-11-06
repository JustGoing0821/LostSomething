// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSCharacterChoiceController.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "Game/LSCharacterChooseGameMode.h"
#include "UserInterface/Network/CharacterChooseWidget.h"
#include "UserInterface/Network/VRReadyWidget.h"
#include <Game/LSGameInstance.h>

ALSCharacterChoiceController::ALSCharacterChoiceController()
{
	static ConstructorHelpers::FClassFinder<UCharacterChooseWidget> CharacterChooseWidgetRef(TEXT("/Game/UI/Network/BP_CharacterChooseWidget.BP_CharacterChooseWidget_C"));
	if (CharacterChooseWidgetRef.Class)
	{
		CharacterChooseWidgetClass = CharacterChooseWidgetRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UVRReadyWidget> VRReadyWidgetRef(TEXT("/Game/UI/Network/WBP_VRReadyWidget.WBP_VRReadyWidget_C"));
	if (VRReadyWidgetRef.Class)
	{
		VRReadyWidgetClass = VRReadyWidgetRef.Class;
	}

	SetShowMouseCursor(true);
	bReplicates = true;
}

void ALSCharacterChoiceController::BeginPlay()
{
	Super::BeginPlay();

	ULSGameInstance* GI = Cast<ULSGameInstance>(GetWorld()->GetGameInstance());

	if (GI->isVR == true)
	{
		if (IsLocalController() && VRReadyWidgetClass)
		{
			VRReadyWidget = CreateWidget<UVRReadyWidget>(this, VRReadyWidgetClass);
			if (VRReadyWidget)
			{
				VRReadyWidget->AddToViewport(1);
			}

			//VRReadyWidget->OnCharacterChoose.BindUObject(this, &ALSCharacterChoiceController::OnCharacterChoose);
			//VRReadyWidget->OnCharacterChoose.BindUObject(this, &ALSCharacterChoiceController::OnCharacterChoose);
		}

		ALSCharacterChooseGameMode* GameMode = Cast<ALSCharacterChooseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->CharacterChoiceChanged.AddUObject(this, &ALSCharacterChoiceController::UpdateCharacterChooseWidget);
		}
	}
	else {
		if (IsLocalController() && CharacterChooseWidgetClass)
		{
			CharacterChooseWidget = CreateWidget<UCharacterChooseWidget>(this, CharacterChooseWidgetClass);
			if (CharacterChooseWidget)
			{
				CharacterChooseWidget->AddToViewport(1);
			}

			CharacterChooseWidget->OnCharacterChoose.BindUObject(this, &ALSCharacterChoiceController::OnCharacterChoose);
			CharacterChooseWidget->OnCharacterChoose.BindUObject(this, &ALSCharacterChoiceController::OnCharacterChoose);
		}

		ALSCharacterChooseGameMode* GameMode = Cast<ALSCharacterChooseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->CharacterChoiceChanged.AddUObject(this, &ALSCharacterChoiceController::UpdateCharacterChooseWidget);
		}
	}
}

void ALSCharacterChoiceController::OnCharacterChoose(ELSCharacterChoice InCharacterChoice)
{
	if (HasAuthority())
	{
		SetCharacterChoice(true, InCharacterChoice);
	}
	else
	{
		ServerRPCSetCharacterChoice(InCharacterChoice);
	}
}

void ALSCharacterChoiceController::SetCharacterChoice(bool bisServer, ELSCharacterChoice InCharacterChoice)
{
	if (HasAuthority())
	{
		ALSCharacterChooseGameMode* GameMode = Cast<ALSCharacterChooseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->SetCharacterChoice(bisServer, InCharacterChoice);
		}
	}
}

void ALSCharacterChoiceController::UpdateCharacterChooseWidget(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{
	if (IsLocalController())
	{
		if (CharacterChooseWidget)
		{
			CharacterChooseWidget->UpdateCharacterChooseWidget(ServerChoice, ClientChoice);
		}
	}
	else
	{
		ClientRPCUpdateCharacterChooseWidget(ServerChoice, ClientChoice);
	}

}

void ALSCharacterChoiceController::ServerRPCSetCharacterChoice_Implementation(ELSCharacterChoice InCharacterChoice)
{
	SetCharacterChoice(false, InCharacterChoice);
}

void ALSCharacterChoiceController::ClientRPCUpdateCharacterChooseWidget_Implementation(ELSCharacterChoice ServerChoice, ELSCharacterChoice ClientChoice)
{
	if (IsLocalController())
	{
		if (CharacterChooseWidget)
		{
			CharacterChooseWidget->UpdateCharacterChooseWidget(ServerChoice, ClientChoice);
		}
	}
}
