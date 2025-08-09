// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LS2DDragPuzzleWidget.generated.h"

DECLARE_DELEGATE(FOnPuzzlePressedDelegate);
DECLARE_DELEGATE(FOnPuzzleReleasedDelegate);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULS2DDragPuzzleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULS2DDragPuzzleWidget(const FObjectInitializer& ObjectInitializer);

	FOnPuzzlePressedDelegate OnPuzzlePressed;
	FOnPuzzleReleasedDelegate OnPuzzleReleased;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetBallLocation(FVector2D InCursorPos);

protected:
	UPROPERTY()
	TObjectPtr<class UImage> ImgGoal1;

	UPROPERTY()
	TObjectPtr<class UButton> BtnBall1;

	UFUNCTION()
	void OnImagePressed();

	UFUNCTION()
	void OnImageReleased();
};
