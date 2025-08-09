// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

// Cursor Section
public:
	//FORCEINLINE void SetCursorPosition(float InCursorX, float InCursorY) { SiJaeCursorX = InCursorX; SiJaeCursorY = InCursorY; }
	void SetCursorPosition(float InCursorX, float InCursorY);

protected:
	float SiJaeCursorX;

	float SiJaeCursorY;
};
