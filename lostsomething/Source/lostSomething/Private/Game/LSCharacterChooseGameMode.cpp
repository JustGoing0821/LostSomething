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

void ALSCharacterChooseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer) return;

	auto GI = Cast<ULSGameInstance>(NewPlayer->GetGameInstance());
	if (!GI) return;

	PlayerCount++;

	// 첫 번째 플레이어 = 서버 (호스트)
	if (PlayerCount == 1)
	{
		ServerPlayerNickName = GI->ServerNickName;
		UE_LOG(LogTemp, Warning, TEXT("Server player joined: %s"), *ServerPlayerNickName);
	}
	// 두 번째 플레이어 = 클라이언트
	else if (PlayerCount == 2)
	{
		ClientPlayerNickName = GI->ClientNickName;
		UE_LOG(LogTemp, Warning, TEXT("Client player joined: %s"), *ClientPlayerNickName);

		// 이제 양쪽 닉네임을 모두 알았으니 모든 플레이어에게 동기화
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (PC)
			{
				auto PlayerGI = Cast<ULSGameInstance>(PC->GetGameInstance());
				if (PlayerGI)
				{
					PlayerGI->ServerNickName = ServerPlayerNickName;
					PlayerGI->ClientNickName = ClientPlayerNickName;

					UE_LOG(LogTemp, Warning, TEXT("Updated player GI - Server: %s, Client: %s"),
						*ServerPlayerNickName, *ClientPlayerNickName);
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("All nicknames synced!"));
	}
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

void ALSCharacterChooseGameMode::SetPlayerNickName(APlayerController* PC, const FString& NickName)
{
	if (!PC) return;

	// 서버인지 클라이언트인지 확인
	if (PC->GetLocalRole() == ROLE_Authority && PC->IsLocalController())
	{
		ServerPlayerNickName = NickName;
	}
	else
	{
		ClientPlayerNickName = NickName;
	}

	// 닉네임이 모두 설정되면 브로드캐스트
	BroadcastNickNames();
}

void ALSCharacterChooseGameMode::BroadcastNickNames()
{
	// 모든 플레이어 컨트롤러에게 닉네임 전송
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			auto GI = Cast<ULSGameInstance>(PC->GetGameInstance());
			if (GI)
			{
				GI->ServerNickName = ServerPlayerNickName;
				GI->ClientNickName = ClientPlayerNickName;
			}
		}
	}
}