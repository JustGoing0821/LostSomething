// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRReadyWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API UVRReadyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//=============================================
	//     PROPERTIES & VARIABLES
	//=============================================
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ReadyPC;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* btn_ReadyVR;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* txt_ReadyPC;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* txt_ReadyVR;

	//=============================================
	//     FUCTIONS
	//=============================================
	virtual void NativeConstruct() override;

	void ReadyPC();

	void ReadyVR();
	
};
