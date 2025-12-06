// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LSCharacterChooseGameMode.h"
#include "lostSomething.h"
#include "UserInterface/Network/ChooseCharacterEndWidget.h"
#include "Game/LSGameInstance.h"

ALSCharacterChooseGameMode::ALSCharacterChooseGameMode()
{
	//Player Controller Class
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/lostSomething.LSCharacterChoiceController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("PlayerControllerClassRef Not Found"));
	}

	//Widget
	static ConstructorHelpers::FClassFinder<UChooseCharacterEndWidget> GameStartWidgetRef(TEXT("/Game/UI/Network/WBP_ChooseCharacterEnd.WBP_ChooseCharacterEnd_C"));
	if (GameStartWidgetRef.Class)
	{
		GameStartWidgetClass = GameStartWidgetRef.Class;
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("GameStartWidgetClass Null"));
	}

	CharacterChoices.Add(ELSNetworkPosition::Server, ELSCharacterChoice::None);
	CharacterChoices.Add(ELSNetworkPosition::Client, ELSCharacterChoice::None);
}

void ALSCharacterChooseGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (GameStartWidgetClass)
	{
		GameStartWidget = CreateWidget<UChooseCharacterEndWidget>(GetWorld(), GameStartWidgetClass);
		if (GameStartWidget)
		{
			GameStartWidget->AddToViewport(5);
			GameStartWidget->OnCharacterChooseEnd.BindUObject(this, &ALSCharacterChooseGameMode::GameStart);
		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("%s"), TEXT("GameStartWidget Null"));
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("GameStartWidgetClass Null"));
	}
}

void ALSCharacterChooseGameMode::SetCharacterChoice(bool bisServer, ELSCharacterChoice InCharacterChoice)
{
	if (bisServer)
	{
		CharacterChoices[ELSNetworkPosition::Server] = InCharacterChoice;
	}
	else
	{
		CharacterChoices[ELSNetworkPosition::Client] = InCharacterChoice;
	}

	CharacterChoiceChanged.Broadcast(CharacterChoices[ELSNetworkPosition::Server], CharacterChoices[ELSNetworkPosition::Client]);


	//Log
	FString EnumString1 = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoices[ELSNetworkPosition::Server])).ToString();
	FString EnumString2 = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(CharacterChoices[ELSNetworkPosition::Client])).ToString();
	//LS_LOG(LogLS, Log, TEXT("Server Choice : %s, ClientChoice : %s"), *EnumString1, *EnumString2);
}

void ALSCharacterChooseGameMode::GameStart()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	ELSCharacterChoice ServerChoice = CharacterChoices[ELSNetworkPosition::Server];
	ELSCharacterChoice ClientChoice = CharacterChoices[ELSNetworkPosition::Client];

	if (ServerChoice != ELSCharacterChoice::None && ClientChoice != ELSCharacterChoice::None && ServerChoice != ClientChoice)
	{
		ULSGameInstance* GameInstance = Cast<ULSGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->SetCharacterChoices(ServerChoice, ClientChoice);
			FString MapUrl = GameInstance->GetChooseLevelUrl();
			if (!MapUrl.IsEmpty())
			{
				GetWorld()->ServerTravel(MapUrl);
			}
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("%s"), TEXT("Character Choose Error"));
	}
}
