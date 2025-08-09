// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DPuzzleHUD.h"
#include "lostSomething.h"
#include "Components/Image.h"
#include "Blueprint/SlateBlueprintLibrary.h"

ULS2DPuzzleHUD::ULS2DPuzzleHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SiJaeCursorX = 0.f;
	SiJaeCursorY = 0.f;
}

void ULS2DPuzzleHUD::NativeConstruct()
{
	Super::NativeConstruct();

	ImgCursor = Cast<UImage>(GetWidgetFromName(TEXT("img_cursor")));
	ensure(ImgCursor);
}

void ULS2DPuzzleHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// ½ºÅ©¸° ÁÂÇ¥¸¦ À§Á¬ ·ÎÄÃ ÁÂÇ¥·Î º¯È¯
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::ScreenToViewport(GetOwningPlayer(), FVector2D(SiJaeCursorX, SiJaeCursorY), ViewportPosition);

	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
	ImgCursor->SetRenderTranslation(ViewportPosition);
}

void ULS2DPuzzleHUD::SetCursorPosition(float InCursorX, float InCursorY)
{
	SiJaeCursorX = InCursorX;
	SiJaeCursorY = InCursorY;
	//LS_WDGLOG(LogLS, Log, TEXT("Begin : %f, %f"), SiJaeCursorX, SiJaeCursorY);
}
