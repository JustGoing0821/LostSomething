// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSDarkWidget.h"
#include "Components/Image.h"

void ULSDarkWidget::SetOpacityByDistance(float Distance)
{
	if (!DarkImage) return;

	const float MinDistance = 200.0f;
	const float MaxDistance = 1000.0f;

	float Alpha = 0.0f;

	if (Distance > MinDistance)
	{
		Alpha = FMath::Clamp((Distance - MinDistance) / (MaxDistance - MinDistance), 0.0f, 1.0f);
	}

	FLinearColor NewColor = DarkImage->ColorAndOpacity;
	NewColor.A = Alpha;

	DarkImage->SetColorAndOpacity(NewColor);
}
