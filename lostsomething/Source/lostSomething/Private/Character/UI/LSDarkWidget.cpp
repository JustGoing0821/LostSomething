// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSDarkWidget.h"
#include "Components/Image.h"

void ULSDarkWidget::SetOpacityByDistance(float Distance)
{
	if (!DarkImage) return;

	const float MinDistance = 200.0f;
	const float MaxDistance = 1500.0f;

	float Alpha = 0.0f;

	if (Distance > MinDistance)
	{
		float Normalized = FMath::Clamp((Distance - MinDistance) / (MaxDistance - MinDistance), 0.0f, 1.0f);
		Alpha = Normalized * MaxOpacity;
	}

	FLinearColor NewColor = DarkImage->ColorAndOpacity;
	NewColor.A = Alpha;

	DarkImage->SetColorAndOpacity(NewColor);
}
