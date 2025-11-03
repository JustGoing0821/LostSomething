// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/VRReadyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UVRReadyWidget::NativeConstruct()
{
	btn_ReadyPC->OnClicked.AddDynamic(this, &UVRReadyWidget::ReadyPC);
	btn_ReadyVR->OnClicked.AddDynamic(this, &UVRReadyWidget::ReadyVR);

	if (txt_ReadyPC)
	{
		txt_ReadyPC->SetText(FText::FromString(TEXT("Not Ready")));
	}
	if (txt_ReadyVR)
	{
		txt_ReadyVR->SetText(FText::FromString(TEXT("Not Ready")));
	}
}

void UVRReadyWidget::ReadyPC()
{
}

void UVRReadyWidget::ReadyVR()
{
}
