// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LTPlayerController.generated.h"

UENUM(BlueprintType)
enum class ECharacterChoice : uint8
{
	None = 0,
	SiJae,
	IJae
};

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALTPlayerController();

	UPROPERTY(EditAnywhere, Replicated)
	ECharacterChoice CharacterChoice = ECharacterChoice::None;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
