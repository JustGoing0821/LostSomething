// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Puzzle/UI/LS2DWidgetProperty.h"
#include "LS2DPuzzleHUD.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULS2DPuzzleHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULS2DPuzzleHUD(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


protected:
	UPROPERTY()
	TObjectPtr<class UImage> ImgCursor;

	UPROPERTY()
	TObjectPtr<class ULS2DDragPuzzleWidget> DragPuzzleWidget;

	UPROPERTY()
	TObjectPtr<class UButton> BtnExit;

	UPROPERTY()
	TObjectPtr<class ULS2DPuzzleTimerWidget> PuzzleTimerWidget;

	UPROPERTY()
	TObjectPtr<class ULSInformationWidget> InformationWidget;

	UFUNCTION()
	void OnBtnExitClicked();

// Cursor Section
public:
	//FORCEINLINE void SetCursorPosition(float InCursorX, float InCursorY) { SiJaeCursorX = InCursorX; SiJaeCursorY = InCursorY; }
	void SetCursorPosition(FVector2D InCursorPos);

	FORCEINLINE void SetbIsSiJaeDragging(uint8 InIsSiJaeDragging) { bIsSiJaeDragging = InIsSiJaeDragging; }

protected:
	FVector2D SiJaeCursorPos;
	uint8 bIsSiJaeDragging : 1;

// Timer Section
public:
	void UpdateTimer(float Timer);

// Widget Section
public:
	ELS2DWidgetProperty CurrentActivateWidget;

	void OnStartWidget(const FName& InDataName);
};
