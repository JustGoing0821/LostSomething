// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/UI/LS2DDragPuzzleWidget.h"
#include "lostSomething.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/Button.h"

ULS2DDragPuzzleWidget::ULS2DDragPuzzleWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULS2DDragPuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ImgGoal1 = Cast<UImage>(GetWidgetFromName(TEXT("img_goal1")));
	ensure(ImgGoal1);

	BtnBall1 = Cast<UButton>(GetWidgetFromName(TEXT("btn_ball1")));
	ensure(BtnBall1);

	if (BtnBall1)
	{
		BtnBall1->OnPressed.AddDynamic(this, &ULS2DDragPuzzleWidget::OnImagePressed);
		BtnBall1->OnReleased.AddDynamic(this, &ULS2DDragPuzzleWidget::OnImageReleased);
	}
}

void ULS2DDragPuzzleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void ULS2DDragPuzzleWidget::SetBallLocation(FVector2D InCursorPos)
{
	BtnBall1->SetRenderTranslation(InCursorPos);
}


void ULS2DDragPuzzleWidget::OnImagePressed()
{
	OnPuzzlePressed.ExecuteIfBound();
}

void ULS2DDragPuzzleWidget::OnImageReleased()
{
	OnPuzzleReleased.ExecuteIfBound();
}
