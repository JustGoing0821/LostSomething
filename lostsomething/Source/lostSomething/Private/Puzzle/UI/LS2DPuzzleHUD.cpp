// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "lostSomething.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/AssetManager.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Puzzle/UI/LS2DDragPuzzleWidget.h"
#include "Puzzle/UI/LS2DPuzzleTimerWidget.h"
#include "Puzzle/UI/LSInformationWidget.h"
#include "Level/LSInformationData.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interface/LS2DPuzzleControllerInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"


ULS2DPuzzleHUD::ULS2DPuzzleHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SiJaeCursorPos = FVector2D(0.f, 0.f);
	bIsSiJaeDragging = false;
	CurrentActivateWidget = ELS2DWidgetProperty::None;
}

void ULS2DPuzzleHUD::NativeConstruct()
{
	Super::NativeConstruct();

	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	ImgCursor = Cast<UImage>(GetWidgetFromName(TEXT("img_cursor")));
	if (ImgCursor) ImgCursor->SetVisibility(ESlateVisibility::Hidden);
	else LS_WDGLOG(LogLS, Error, TEXT("%s"), TEXT("No ImgCursor"));

	BtnExit = Cast<UButton>(GetWidgetFromName(TEXT("btn_exit")));
	if (BtnExit)
	{
		BtnExit->OnClicked.AddDynamic(this, &ULS2DPuzzleHUD::OnBtnExitClicked);
	}

	DragPuzzleWidget = Cast<ULS2DDragPuzzleWidget>(GetWidgetFromName(TEXT("wbp_drag_puzzle")));
	if (DragPuzzleWidget) DragPuzzleWidget->SetVisibility(ESlateVisibility::Hidden);
	else LS_WDGLOG(LogLS, Error, TEXT("%s"), TEXT("No DragPuzzleWidget"));

	PuzzleTimerWidget = Cast<ULS2DPuzzleTimerWidget>(GetWidgetFromName(TEXT("wbp_puzzle_timer")));
	if (PuzzleTimerWidget) PuzzleTimerWidget->SetVisibility(ESlateVisibility::Hidden);
	else LS_WDGLOG(LogLS, Error, TEXT("%s"), TEXT("No PuzzleTimerWidget"));

	InformationWidget = Cast<ULSInformationWidget>(GetWidgetFromName(TEXT("wbp_information")));
	if (InformationWidget) InformationWidget->SetVisibility(ESlateVisibility::Hidden);
	else LS_WDGLOG(LogLS, Error, TEXT("%s"), TEXT("No InformationWidget"));
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
	//LS_WDGLOG(LogLS, Log, TEXT("Begin"));

	ILS2DPuzzleControllerInterface* PuzzleInterface = Cast<ILS2DPuzzleControllerInterface>(GetOwningPlayer());

	if (CurrentActivateWidget == ELS2DWidgetProperty::Information)
	{
		if (PuzzleInterface)
		{
			PuzzleInterface->OnExit2DPuzzle(false);
		}
	}
	else if (CurrentActivateWidget == ELS2DWidgetProperty::Drag)
	{
		if (PuzzleInterface)
		{
			PuzzleInterface->OnExit2DPuzzle(true);
		}
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

void ULS2DPuzzleHUD::OnStartWidget(const FName& InDataName, const FVector2D& InGoalPos)
{
	//LS_WDGLOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//LS_WDGLOG(LogLSls, Log, TEXT("InGoalPos : %f, %f"), InGoalPos.X, InGoalPos.Y);

	if (InDataName == TEXT("Step"))
	{
		CurrentActivateWidget = ELS2DWidgetProperty::Drag;
		if (DragPuzzleWidget)
		{
			DragPuzzleWidget->SetVisibility(ESlateVisibility::Visible);
			DragPuzzleWidget->OnStartDragPuzzle(InGoalPos);
		}

		if (PuzzleTimerWidget) PuzzleTimerWidget->SetVisibility(ESlateVisibility::Visible);
		if (InformationWidget) InformationWidget->SetVisibility(ESlateVisibility::Hidden);

		if (Cast<ILSCharacterChoiceInterface>(GetOwningPlayer())->GetCharacterChoice() == ELSCharacterChoice::IJae)
		{
			ImgCursor->SetVisibility(ESlateVisibility::Visible);
		}

		return;
	}

	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("LSInformationData"), Assets);

	if (0 < Assets.Num())
	{
		for (const FPrimaryAssetId& AssetId : Assets)
		{
			if (AssetId.PrimaryAssetName == InDataName)
			{
				FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
				//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

				if (AssetPtr.IsPending())
				{
					AssetPtr.LoadSynchronous();
				}

				ULSInformationData* DataAsset = Cast<ULSInformationData>(AssetPtr.Get());
				if (DataAsset)
				{
					CurrentActivateWidget = DataAsset->GetWidgetProperty();
					if (InformationWidget) InformationWidget->UpdateInfo(DataAsset->GetInformationData());
					break;
				}
			}
		}
	}

	if (DragPuzzleWidget) DragPuzzleWidget->SetVisibility(ESlateVisibility::Hidden);
	if (PuzzleTimerWidget) PuzzleTimerWidget->SetVisibility(ESlateVisibility::Hidden);
	if (InformationWidget) InformationWidget->SetVisibility(ESlateVisibility::Visible);
}
