// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DDragPuzzleWidget.h"
#include "lostSomething.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LSSiJaeCursorDragInterface.h"
#include "Interface/LS2DPuzzleControllerInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"

ULS2DDragPuzzleWidget::ULS2DDragPuzzleWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GoalPos = FVector2D(0, 0);
}

void ULS2DDragPuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ImgGoal1 = Cast<UImage>(GetWidgetFromName(TEXT("img_goal1")));
	ensure(ImgGoal1);
	ImgGoal1->SetOpacity(0.0f);

	ImgPiece1 = Cast<UImage>(GetWidgetFromName(TEXT("img_piece1")));
	ensure(ImgPiece1);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgGoal1->Slot))
			{
				FVector2D CanvasSize = ImgGoal1->GetParent()->GetCachedGeometry().GetLocalSize();
				CanvasSlot->SetSize(CanvasSize * 0.1f);
				//CanvasSlot->SetPosition(CanvasSize*0.1f);
			}
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgPiece1->Slot))
			{
				FVector2D CanvasSize = ImgPiece1->GetParent()->GetCachedGeometry().GetLocalSize();
				CanvasSlot->SetSize(CanvasSize * 0.1f);
				CanvasSlot->SetPosition(CanvasSize * 0.5f);
			}
		}
	), 0.1f, false);
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
		ILSSiJaeCursorDragInterface* PC = Cast<ILSSiJaeCursorDragInterface>(GetOwningPlayer());
		if (PC)
		{
			PC->OnChangeSiJaeDragState(true);
		}
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
		ILSSiJaeCursorDragInterface* PCCursor = Cast<ILSSiJaeCursorDragInterface>(GetOwningPlayer());
		if (PCCursor)
		{
			PCCursor->OnChangeSiJaeDragState(false);
		}

		if (IsMouseOverImage(ImgGoal1, InMouseEvent))
		{
			ILS2DPuzzleControllerInterface* PCPuzzle = Cast<ILS2DPuzzleControllerInterface>(GetOwningPlayer());
			if (PCPuzzle)
			{
				PCPuzzle->OnClear2DPuzzle();
			}
		}
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

void ULS2DDragPuzzleWidget::OnStartDragPuzzle(const FVector2D& InGoalPos)
{
	//LS_WDGLOG(LogLSls, Log, TEXT("InGoalPos : %f, %f"), InGoalPos.X, InGoalPos.Y);
	GoalPos = InGoalPos;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ImgGoal1->Slot))
			{
				FVector2D CanvasSize = ImgGoal1->GetParent()->GetCachedGeometry().GetLocalSize();
				GoalPos.X = CanvasSize.X * GoalPos.X;
				GoalPos.Y = CanvasSize.Y * GoalPos.Y;
				CanvasSlot->SetPosition(GoalPos);
				//LS_WDGLOG(LogLSls, Log, TEXT("CanvasSize : %f, %f"), CanvasSize.X, CanvasSize.Y);
				//LS_WDGLOG(LogLSls, Log, TEXT("InGoalPos : %f, %f"), GoalPos.X, GoalPos.Y);
				//LS_WDGLOG(LogLSls, Log, TEXT("GoalPos : %f, %f"), GoalPos.X, GoalPos.Y);
			}
			else
			{
				LS_WDGLOG(LogLSls, Error, TEXT("%s"), TEXT("No ImgGoal1!!"));
			}

			if (Cast<ILSCharacterChoiceInterface>(GetOwningPlayer())->GetCharacterChoice() == ELSCharacterChoice::IJae)
			{
				ImgGoal1->SetOpacity(1.0f);
			}
		}
	), 1.f, false);
}

bool ULS2DDragPuzzleWidget::IsMouseOverImage(UImage* TargetImage, const FPointerEvent& MouseEvent)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (!TargetImage)
		return false;

	FGeometry ImageGeometry = TargetImage->GetCachedGeometry();
	FVector2D LocalMousePos = ImageGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	FVector2D ImageSize = ImageGeometry.GetLocalSize();

	return (LocalMousePos.X >= 0 && LocalMousePos.X <= ImageSize.X &&
		LocalMousePos.Y >= 0 && LocalMousePos.Y <= ImageSize.Y);
}