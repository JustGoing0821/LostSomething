// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JY_GameMode.generated.h"




UCLASS()
class LOSTSOMETHING_API AJY_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AJY_GameMode();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

protected:
	TSubclassOf<APawn> IJaePawnClass;
	TSubclassOf<APawn> SiJaePawnClass;

public:
	
};


/**
 * 
 */
//UCLASS()
//class LOSTSOMETHING_API AJY_GameMode : public AGameModeBase
//{
//	GENERATED_BODY()
//	
//	void HostLANGmae();
//
//	void JoinLANGmae();
//
//};
