// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void ULSHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("ULSHUDWidget::NativeConstruct() Called"));

    // 초기 슬롯 색상 설정
    UpdateSlotBorderColors(0);

	TxtAim = Cast<UTextBlock>(GetWidgetFromName(TEXT("txt_aim")));
	ensure(TxtAim);
}

void ULSHUDWidget::UpdateHpBar(float NewHp)
{
    if (PbHpBar)
    {
        const float Percent = NewHp / 100.0f;
        PbHpBar->SetPercent(Percent);
        UE_LOG(LogTemp, Warning, TEXT("hud : HpBar updated: %.1f%%"), Percent * 100.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HUD : PbHpBar is nullptr!"));
    }
}



void ULSHUDWidget::UpdateSlotBorderColors(int32 NewSelectedSlot)
{
	// 매개변수가 전달되면 해당 값 사용, 아니면 기존 SelectedSlot 사용
	int32 SlotToSelect = (NewSelectedSlot >= 0) ? NewSelectedSlot : SelectedSlot;


	// 모든 슬롯을 기본 색상으로 설정
	for (int32 i = 0; i <= MaxSlots; ++i)
	{
		SetSlotBorderColor(i, UnselectedSlotColor);
	}

	// 선택된 슬롯만 하이라이트 색상으로 설정

	//SetSlotBorderColor(SelectedSlot, SelectedSlotColor);
	

	// SelectedSlot 업데이트
	SelectedSlot = SlotToSelect;

	// 선택된 슬롯만 하이라이트 색상으로 설정
	SetSlotBorderColor(SlotToSelect, SelectedSlotColor);


	UE_LOG(LogTemp, Warning, TEXT("Slot border colors updated. Selected slot: %d"), SelectedSlot);

	
	


}



void ULSHUDWidget::SetIcon(int32 CurrentSlot, UTexture2D* ItemIcon)
{
	UE_LOG(LogTemp, Warning, TEXT("ULSHUDWidget::SetIcon() called - Slot: %d"), CurrentSlot);

	UImage* TargetSlotIcon = nullptr;

	switch (CurrentSlot)
	{
	case 0:
		TargetSlotIcon = SlotIcon_0;
		break;
	case 1:
		TargetSlotIcon = SlotIcon_1;
		break;
	case 2:
		TargetSlotIcon = SlotIcon_2;
		break;
	case 3:
		TargetSlotIcon = SlotIcon_3;
		break;
	case 4:
		TargetSlotIcon = SlotIcon_4;
		break;
	
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Invalid slot index: %d"), CurrentSlot);
		return;
	}

	if (TargetSlotIcon)
	{
		if (ItemIcon)
		{
			// 유효한 아이콘이 있을 때 - 아이템 아이콘 표시
			TargetSlotIcon->SetBrushFromTexture(ItemIcon, true);
			TargetSlotIcon->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Warning, TEXT("Item icon set for slot %d"), CurrentSlot);
		}
		else
		{
			// 아이콘이 null일 때 - 기본 슬롯 아이콘 표시
			UTexture2D* BlankIcon = DefaultSlotIcon.LoadSynchronous();
			if (BlankIcon)
			{
				TargetSlotIcon->SetBrushFromTexture(BlankIcon, true);
				TargetSlotIcon->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Warning, TEXT("Default icon set for slot %d"), CurrentSlot);
			}
			else
			{
				
				TargetSlotIcon->SetBrushFromTexture(nullptr, true);
				TargetSlotIcon->SetVisibility(ESlateVisibility::Hidden);
				UE_LOG(LogTemp, Warning, TEXT("Slot %d hidden (no default icon)"), CurrentSlot);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SlotImage_%d widget is null"), CurrentSlot);
	}
}

void ULSHUDWidget::SetSlotBorderColor(int32 SlotIndex, const FLinearColor& Color)
{
	UBorder* TargetBorder = nullptr;

	// 슬롯 인덱스에 따라 해당 Border 위젯 선택
	switch (SlotIndex)
	{
	case 0:
		TargetBorder = SlotBorder_0;
		break;
	case 1:
		TargetBorder = SlotBorder_1;
		break;
	case 2:
		TargetBorder = SlotBorder_2;
		break;
	case 3:
		TargetBorder = SlotBorder_3;
		break;
	case 4:
		TargetBorder = SlotBorder_4;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid slot index: %d"), SlotIndex);
		return;
	}

	// Border 위젯이 유효한 경우 색상 설정
	if (TargetBorder)
	{
		TargetBorder->SetBrushColor(Color);
		UE_LOG(LogTemp, Log, TEXT("Set slot %d border color"), SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SlotBorder_%d is null! Check widget binding."), SlotIndex);
	}
}

void ULSHUDWidget::UpdateAim(const FString& InString)
{
	if (TxtAim)
	{
		TxtAim->SetText(FText::FromString(InString));
	}
}


