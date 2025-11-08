// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/Network/VRReadyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include <Character/Players/LSCharacterChoice.h>
#include <Game/LSCharacterChoiceController.h>

void UVRReadyWidget::NativeConstruct()
{
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

void UVRReadyWidget::ReadyVR()
{
	if (!isReadyVR)
	{
		OnVRReady.ExecuteIfBound(ELSCharacterChoice::SiJae);
		ChangeReady();
	}
	else
	{
		OnVRReady.ExecuteIfBound(ELSCharacterChoice::None);
		ChangeReady();
	}
}

void UVRReadyWidget::ChangeReady()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (OwningPawn)
	{
		AController* Controller = OwningPawn->GetController();
		// 또는 PlayerController로 캐스팅
		ALSCharacterChoiceController* PC = Cast<ALSCharacterChoiceController>(Controller);

		if (PC->GetName() == TEXT("LSCharacterChoiceController_0"))
		{
			if (txt_ReadyPC && !isReadyVR)
			{
				txt_ReadyPC->SetText(FText::FromString(TEXT("Ready")));
				isReadyVR = true;
			}
			else
			{
				txt_ReadyPC->SetText(FText::FromString(TEXT("Not Ready")));
				isReadyVR = false;
			}
		}
		else 
		{
			if (txt_ReadyVR && !isReadyVR)
			{
				txt_ReadyVR->SetText(FText::FromString(TEXT("Ready")));
				isReadyVR = true;
			}
			else
			{
				txt_ReadyVR->SetText(FText::FromString(TEXT("Not Ready")));
				isReadyVR = false;
			}
		}			
	}
}
