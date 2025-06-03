// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"


void ULSHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("ULSHUDWidget::NativeConstruct() Called"));

    // 초기 슬롯 색상 설정
    UpdateSlotBorderColors();
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

void ULSHUDWidget::ChangeSlot(int32 NewSlot)
{
	// 슬롯 범위 검증 및 순환 처리
	if (NewSlot < 0)
	{
		SelectedSlot = MaxSlots;
	}
	else if (NewSlot > MaxSlots)
	{
		SelectedSlot = 0;
	}
	else
	{
		SelectedSlot = NewSlot;
	}

	UE_LOG(LogTemp, Warning, TEXT("Slot changed to: %d"), SelectedSlot);

	// 슬롯 색상 업데이트
	UpdateSlotBorderColors();
}

void ULSHUDWidget::UpdateSlotBorderColors()
{
	// 모든 슬롯을 기본 색상으로 설정
	for (int32 i = 0; i <= MaxSlots; ++i)
	{
		SetSlotBorderColor(i, UnselectedSlotColor);
	}

	// 선택된 슬롯만 하이라이트 색상으로 설정
	SetSlotBorderColor(SelectedSlot, SelectedSlotColor);

	UE_LOG(LogTemp, Warning, TEXT("Slot border colors updated. Selected slot: %d"), SelectedSlot);

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


