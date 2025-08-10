// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DDragPuzzleWidget.h"
#include "lostSomething.h"
//#include "Blueprint/SlateBlueprintLibrary.h"
//#include "Widgets/SWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"

ULS2DDragPuzzleWidget::ULS2DDragPuzzleWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULS2DDragPuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ImgGoal1 = Cast<UImage>(GetWidgetFromName(TEXT("img_goal1")));
	ensure(ImgGoal1);

	ImgPiece1 = Cast<UImage>(GetWidgetFromName(TEXT("img_piece1")));
	ensure(ImgPiece1);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgGoal1->Slot))
			{
				FVector2D CanvasSize = ImgGoal1->GetParent()->GetCachedGeometry().GetLocalSize();
				CanvasSlot->SetSize(CanvasSize * 0.2f);
				CanvasSlot->SetPosition(CanvasSize*0.5f);
			}
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgPiece1->Slot))
			{
				FVector2D CanvasSize = ImgPiece1->GetParent()->GetCachedGeometry().GetLocalSize();
				CanvasSlot->SetSize(CanvasSize * 0.1f);
				CanvasSlot->SetPosition(CanvasSize * 0.2f);
			}
		}
	), 1.f, false);
}

void ULS2DDragPuzzleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply ULS2DDragPuzzleWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (IsMouseOverImage(ImgPiece1, InMouseEvent))
	{
		bIsDragging = true;
		OnDraggingStart.ExecuteIfBound();
	}
	else
	{
		//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("IsMouseOverImage False"));
	}

	return FReply::Handled();
}

FReply ULS2DDragPuzzleWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (bIsDragging)
	{
		bIsDragging = false;
		OnDraggingEnd.ExecuteIfBound();
	}

	return FReply::Handled();
}

void ULS2DDragPuzzleWidget::SetPieceLocation(FVector2D InCursorPos)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//ImgPiece1->SetRenderTranslation(InCursorPos);

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgPiece1->Slot))
	{
		FVector2D CanvasSize = ImgPiece1->GetParent()->GetCachedGeometry().GetLocalSize();

		FVector2D PixelPosition = FVector2D(InCursorPos.X * CanvasSize.X, InCursorPos.Y * CanvasSize.Y);
		CanvasSlot->SetPosition(PixelPosition);
		//LS_WDGLOG(LogLS, Log, TEXT("Begin. SiJaeCursorPos : %f, %f"), InCursorPos.X, InCursorPos.Y);
		//LS_WDGLOG(LogLS, Log, TEXT("Begin. PixelPosition : %f, %f"), PixelPosition.X, PixelPosition.Y);
	}
}

bool ULS2DDragPuzzleWidget::IsMouseOverImage(UImage* TargetImage, const FPointerEvent& MouseEvent)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (!TargetImage)
		return false;

	// 이미지의 지오메트리 정보 가져오기
	FGeometry ImageGeometry = TargetImage->GetCachedGeometry();

	// 마우스 위치를 로컬 좌표로 변환
	FVector2D LocalMousePos = ImageGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

	// 이미지 영역 내에 있는지 확인
	FVector2D ImageSize = ImageGeometry.GetLocalSize();

	return (LocalMousePos.X >= 0 && LocalMousePos.X <= ImageSize.X &&
		LocalMousePos.Y >= 0 && LocalMousePos.Y <= ImageSize.Y);
}