// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "lostSomething.h"
#include "Components/Image.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Puzzle/UI/LS2DDragPuzzleWidget.h"
#include "Puzzle/UI/LS2DPuzzleTimerWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Interface/LS2DPuzzleControllerInterface.h"

ULS2DPuzzleHUD::ULS2DPuzzleHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SiJaeCursorPos = FVector2D(0.f, 0.f);
	bIsSiJaeDragging = false;
}

void ULS2DPuzzleHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ImgCursor = Cast<UImage>(GetWidgetFromName(TEXT("img_cursor")));
	ensure(ImgCursor);

	DragPuzzleWidget = Cast<ULS2DDragPuzzleWidget>(GetWidgetFromName(TEXT("wbp_drag_puzzle")));
	ensure(DragPuzzleWidget);

	BtnExit = Cast<UButton>(GetWidgetFromName(TEXT("btn_exit")));
	ensure(BtnExit);

	InformationWidget = Cast<ULSInformationWidget>(GetWidgetFromName(TEXT("wbp_information")));
	ensure(InformationWidget);

	if (BtnExit)
	{
		BtnExit->OnClicked.AddDynamic(this, &ULS2DPuzzleHUD::OnBtnExitClicked);
	}

	PuzzleTimerWidget = Cast<ULS2DPuzzleTimerWidget>(GetWidgetFromName(TEXT("wbp_puzzle_timer")));
	ensure(PuzzleTimerWidget);
}

void ULS2DPuzzleHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//FVector2D ViewportSize;
	//GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	//FVector2D PixelPosition = FVector2D(SiJaeCursorPos.X * ViewportSize.X, SiJaeCursorPos.Y * ViewportSize.Y);
	//ImgCursor->SetRenderTranslation(PixelPosition);
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgCursor->Slot))
	{
		FVector2D CanvasSize = ImgCursor->GetParent()->GetCachedGeometry().GetLocalSize();

		FVector2D PixelPosition = FVector2D(SiJaeCursorPos.X * CanvasSize.X, SiJaeCursorPos.Y * CanvasSize.Y);
		CanvasSlot->SetPosition(PixelPosition);
		//LS_WDGLOG(LogLS, Log, TEXT("Begin. SiJaeCursorPos : %f, %f"), SiJaeCursorPos.X, SiJaeCursorPos.Y);
		//LS_WDGLOG(LogLS, Log, TEXT("Begin. PixelPosition : %f, %f"), PixelPosition.X, PixelPosition.Y);
	}

	if (bIsSiJaeDragging)
	{
		//LS_WDGLOG(LogLS, Log, TEXT("Begin. SiJaeCursorPos : %f, %f"), SiJaeCursorPos.X, SiJaeCursorPos.Y);
		DragPuzzleWidget->SetPieceLocation(SiJaeCursorPos);
	}
}

void ULS2DPuzzleHUD::OnBtnExitClicked()
{
	LS_WDGLOG(LogLS, Log, TEXT("Begin"));

	ILS2DPuzzleControllerInterface* PuzzleInterface = Cast<ILS2DPuzzleControllerInterface>(GetOwningPlayer());
	if (PuzzleInterface)
	{
		PuzzleInterface->OnExit2DPuzzle();
	}
}

void ULS2DPuzzleHUD::SetCursorPosition(FVector2D InCursorPos)
{
	SiJaeCursorPos = InCursorPos;
	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
}

void ULS2DPuzzleHUD::UpdateTimer(float Timer)
{
	if (PuzzleTimerWidget)
	{
		PuzzleTimerWidget->UpdateTimer(Timer);
	}
}
