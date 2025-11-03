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
		isReadyPC = false;
	}
	if (txt_ReadyVR)
	{
		txt_ReadyVR->SetText(FText::FromString(TEXT("Not Ready")));
		isReadyVR = false;
	}
}

void UVRReadyWidget::ReadyPC()
{

	if (txt_ReadyPC && !isReadyPC)
	{
		txt_ReadyPC->SetText(FText::FromString(TEXT("Ready")));
		isReadyPC = true;
	}
	else {
		txt_ReadyPC->SetText(FText::FromString(TEXT("Not Ready")));
		isReadyPC = false;
	}

}

void UVRReadyWidget::ReadyVR()
{

	if (txt_ReadyVR && !isReadyVR)
	{
		txt_ReadyVR->SetText(FText::FromString(TEXT("Ready")));
		isReadyVR = true;
	}
	else {
		txt_ReadyVR->SetText(FText::FromString(TEXT("Not Ready")));
		isReadyVR = false;
	}

}
