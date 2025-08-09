// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "lostSomething.h"
#include "Components/Image.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Puzzle/UI/LS2DDragPuzzleWidget.h"

ULS2DPuzzleHUD::ULS2DPuzzleHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SiJaeCursorX = 0.f;
	SiJaeCursorY = 0.f;
	bIsDragPuzzlePressed = false;
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
		DragPuzzleWidget->OnPuzzlePressed.BindUObject(this, &ULS2DPuzzleHUD::OnDragPuzzlePressed);
		DragPuzzleWidget->OnPuzzleReleased.BindUObject(this, &ULS2DPuzzleHUD::OnDragPuzzleReleased);
	}
}

void ULS2DPuzzleHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ½ºÅ©¸° ÁÂÇ¥¸¦ À§Á¬ ·ÎÄÃ ÁÂÇ¥·Î º¯È¯
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::ScreenToViewport(GetOwningPlayer(), FVector2D(SiJaeCursorX, SiJaeCursorY), ViewportPosition);

	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
	ImgCursor->SetRenderTranslation(ViewportPosition);

	if (bIsDragPuzzlePressed)
	{
		DragPuzzleWidget->SetBallLocation(FVector2D(SiJaeCursorX, SiJaeCursorY));
	}
}

void ULS2DPuzzleHUD::SetCursorPosition(float InCursorX, float InCursorY)
{
	SiJaeCursorX = InCursorX;
	SiJaeCursorY = InCursorY;
	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
}

void ULS2DPuzzleHUD::OnDragPuzzlePressed()
{
	bIsDragPuzzlePressed = true;
}

void ULS2DPuzzleHUD::OnDragPuzzleReleased()
{
	bIsDragPuzzlePressed = false;
}
