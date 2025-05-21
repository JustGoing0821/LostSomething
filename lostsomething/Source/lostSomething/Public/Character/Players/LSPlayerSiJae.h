// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Players/LSPlayer.h"
#include "LSPlayerSiJae.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSPlayerSiJae : public ALSPlayer
{
	GENERATED_BODY()
	
protected:
	virtual bool CanPushWheelchair() const override;
};