// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "LSHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;



	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PbHpBar;

	// 바인딩용 함수
	void UpdateHpBar(float NewHp);


	

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateSlotBorderColors(int32 NewSelectedSlot);

	// Set Icon함수
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetIcon(int32 CurrentSlot, UTexture2D* ItemIcon);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UFUNCTION(BlueprintCallable)
	void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

	UFUNCTION(BlueprintCallable)
	void ShowStaminaBar(bool bShow);

	FLinearColor DefaultBarColor;

protected:
	// 슬롯 아이콘 이미지 위젯
	// 
	// 빈 슬롯 기본 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Icons")
	TSoftObjectPtr<UTexture2D> DefaultSlotIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotIcon_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotIcon_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotIcon_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotIcon_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotIcon_4;


	// 슬롯 테두리 위젯들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder_4;

	// 슬롯 선택 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SelectedSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxSlots = 4;

	// 슬롯 색상 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Colors")
	FLinearColor SelectedSlotColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot Colors")
	FLinearColor UnselectedSlotColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);

private:
	// 특정 슬롯의 테두리 색상 설정
	void SetSlotBorderColor(int32 SlotIndex, const FLinearColor& Color);


//Aim Section
public:
	void UpdateAim(const FString& InString);

protected:
	UPROPERTY()
	TObjectPtr<class UTextBlock> TxtAim;

};
