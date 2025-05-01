// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/JY_GameMode.h"
#include "lostSomething.h"
#include "Character/Players/LSPlayerController.h"


AJY_GameMode::AJY_GameMode()
{
	// Player Character Class Initialize
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Players/BluePrints/BP_LSPlayer.BP_LSPlayer_C"));
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

	//Player Controller Class
	static ConstructorHelpers::FClassFinder<ALSPlayerController> PlayerControllerClassRef(TEXT("/Script/lostSomething.LSPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("PlayerControllerClassRef Not Found"));
	}

}

//서버 접속시 자동 호출
APlayerController* AJY_GameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin Server"));

	APlayerController* ResultController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	LS_LOG(LogLS, Log, TEXT("Player Controller : %s"), *ResultController->GetName());
	if (ResultController->GetName() == TEXT("LSPlayerController_0"))
	{
		//컨트롤러 0번째 : 서버 : 이제
		DefaultPawnClass = IJaePawnClass;
		//return GetWorld()->SpawnActor<APawn>(IJaePawnClass,GetActorLocation())
	}
	else
	{
		//컨트롤러 1번째 : 클라 : 시제
		DefaultPawnClass = SiJaePawnClass;
	}

	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Server End"));
	return ResultController;
}




//다음 맵으로 넘어갈때 따라가기
//void AJY_GameMode::HostLANGmae()
//{
//	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?Listen");
//
//}
//
// 하드코딩 접속 테스트
//void AJY_GameMode::JoinLANGmae()
//{
//	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
//	if (PC)
//	{
//		PC->ClientTravel("192.168.0.40", TRAVEL_Absolute);
//	}
//
//}
