// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "LSDarkWidget.generated.h"

UCLASS()
class LOSTSOMETHING_API ULSDarkWidget : public UUserWidget
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetOpacityByDistance(float Distance);

	//ÃÖ´ë ¾îµÒ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dark Effect")
	float MaxOpacity = 0.97f;  

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* DarkImage;
};