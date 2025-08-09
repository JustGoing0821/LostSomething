// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "lostSomething.h"
#include "Components/Image.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Puzzle/UI/LS2DDragPuzzleWidget.h"

ULS2DPuzzleHUD::ULS2DPuzzleHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SiJaeCursorPos = FVector2D(0.f, 0.f);
	bIsDragPuzzleDragging = false;
}

void ULS2DPuzzleHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ImgCursor = Cast<UImage>(GetWidgetFromName(TEXT("img_cursor")));
	ensure(ImgCursor);

	DragPuzzleWidget = Cast<ULS2DDragPuzzleWidget>(GetWidgetFromName(TEXT("wbp_drag_puzzle")));
	ensure(DragPuzzleWidget);

	if (DragPuzzleWidget)
	{
		DragPuzzleWidget->OnDraggingStart.BindUObject(this, &ULS2DPuzzleHUD::OnDraggingStart);
		DragPuzzleWidget->OnDraggingEnd.BindUObject(this, &ULS2DPuzzleHUD::OnDraggingEnd);
	}
}

void ULS2DPuzzleHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ½ºÅ©¸° ÁÂÇ¥¸¦ À§Á¬ ·ÎÄÃ ÁÂÇ¥·Î º¯È¯
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::ScreenToViewport(GetOwningPlayer(), SiJaeCursorPos, ViewportPosition);

	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
	ImgCursor->SetRenderTranslation(ViewportPosition);

	if (bIsDragPuzzleDragging)
	{
		DragPuzzleWidget->SetBallLocation(SiJaeCursorPos);
	}
}

void ULS2DPuzzleHUD::SetCursorPosition(FVector2D InCursorPos)
{
	SiJaeCursorPos = InCursorPos;
	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
}

void ULS2DPuzzleHUD::OnDraggingStart()
{
	LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	bIsDragPuzzleDragging = true;
}

void ULS2DPuzzleHUD::OnDraggingEnd()
{
	LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	bIsDragPuzzleDragging = false;
}
