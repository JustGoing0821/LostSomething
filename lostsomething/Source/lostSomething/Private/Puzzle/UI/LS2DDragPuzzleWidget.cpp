// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DDragPuzzleWidget.h"
#include "lostSomething.h"
//#include "Blueprint/SlateBlueprintLibrary.h"
//#include "Widgets/SWidget.h"
#include "Components/Image.h"

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
}

void ULS2DDragPuzzleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply ULS2DDragPuzzleWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (IsMouseOverImage(ImgPiece1, InMouseEvent))
	{
		bIsDragging = true;
		OnDraggingStart.ExecuteIfBound();

		return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	else
	{
		LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("IsMouseOverImage False"));
		return FReply::Handled();
	}
}

FReply ULS2DDragPuzzleWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (bIsDragging)
	{
		bIsDragging = false;
		OnDraggingEnd.ExecuteIfBound();
	}
	return FReply::Handled().ReleaseMouseCapture();
}

void ULS2DDragPuzzleWidget::SetBallLocation(FVector2D InCursorPos)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ImgPiece1->SetRenderTranslation(InCursorPos);
}

bool ULS2DDragPuzzleWidget::IsMouseOverImage(UImage* TargetImage, const FPointerEvent& MouseEvent)
{
	LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
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