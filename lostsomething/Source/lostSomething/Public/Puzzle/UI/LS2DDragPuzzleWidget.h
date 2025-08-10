// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LS2DDragPuzzleWidget.generated.h"

DECLARE_DELEGATE(FOnDraggingStartDelegate);
DECLARE_DELEGATE(FOnDraggingEndDelegate);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULS2DDragPuzzleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ULS2DDragPuzzleWidget(const FObjectInitializer& ObjectInitializer);

	FOnDraggingStartDelegate OnDraggingStart;
	FOnDraggingEndDelegate OnDraggingEnd;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	void SetPieceLocation(FVector2D InCursorPos);

protected:
	UPROPERTY()
	TObjectPtr<class UImage> ImgGoal1;

	UPROPERTY()
	TObjectPtr<class UImage> ImgPiece1;

	bool bIsDragging;

	bool IsMouseOverImage(UImage* TargetImage, const FPointerEvent& MouseEvent);
};
