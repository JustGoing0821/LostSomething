// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSSijaeCursorPosInterface.h"
#include "LT2DPuzzleTestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALT2DPuzzleTestGameMode : public AGameModeBase, public ILSSijaeCursorPosInterface
{
	GENERATED_BODY()
	
public:
	ALT2DPuzzleTestGameMode();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character, Meta = (AllowPrivateAccess = "true"))
	bool bIsSiJaeServer;

// 2D Puzzle Section
public:
	FORCEINLINE virtual const FVector2D& GetSiJaeCursorPos() override { return SiJaeCursorPos; }
	FORCEINLINE virtual void SetSiJaeCursorPos(const FVector2D& InSiJaeCursorPos) override { SiJaeCursorPos = InSiJaeCursorPos; }

protected:
	FVector2D SiJaeCursorPos;
};
