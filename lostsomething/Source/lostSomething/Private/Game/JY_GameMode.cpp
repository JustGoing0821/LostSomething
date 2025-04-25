// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/JY_GameMode.h"
#include "lostSomething.h"


AJY_GameMode::AJY_GameMode()
{
	// Player Character Class Initialize
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Level/TestPlayer/BP_LTPlayer.BP_LTPlayer_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> IJaePawnClassRef(TEXT("/Game/Level/TestPlayer/BP_LTPlayerIJae.BP_LTPlayerIJae_C"));
	if (IJaePawnClassRef.Class != NULL)
	{
		IJaePawnClass = IJaePawnClassRef.Class;
	} 

	static ConstructorHelpers::FClassFinder<APawn> SiJaePawnClassRef(TEXT("/Game/Level/TestPlayer/BP_LTPlayerSiJae.BP_LTPlayerSiJae_C"));
	if (SiJaePawnClassRef.Class != NULL)
	{
		SiJaePawnClass = SiJaePawnClassRef.Class;
	}

}

APlayerController* AJY_GameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	APlayerController* ResultController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	LS_LOG(LogLS, Log, TEXT("Player Controller : %s"), *ResultController->GetName());
	if (ResultController->GetName() == TEXT("PlayerController_0"))
	{
		DefaultPawnClass = IJaePawnClass;
	}
	else
	{
		DefaultPawnClass = SiJaePawnClass;
	}



	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
	return ResultController;
}



//void AJY_GameMode::HostLANGmae()
//{
//	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?Listen");
//
//}
//
//void AJY_GameMode::JoinLANGmae()
//{
//	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
//	if (PC)
//	{
//		PC->ClientTravel("192.168.0.40", TRAVEL_Absolute);
//	}
//
//}
