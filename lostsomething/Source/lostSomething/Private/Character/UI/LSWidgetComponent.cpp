// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/UI/LSWidgetComponent.h"
#include "Character/UI/LSUserWidget.h"

void ULSWidgetComponent::InitWidget()
{
	Super::InitWidget();

	ULSUserWidget* LSUserWidget = Cast<ULSUserWidget>(GetWidget());
	if (LSUserWidget)
	{
		LSUserWidget->SetOwningActor(GetOwner());
	}
}
