// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Puzzle/UI/LS2DWidgetProperty.h"
#include "LSInformationData.generated.h"

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSInformationData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("LSInformationData", GetFName());
	}

	FORCEINLINE ELS2DWidgetProperty GetWidgetProperty() const { return WidgetProperty; }
	FORCEINLINE const FString& GetInformationScript() const { return InformationData; }
	FORCEINLINE const TObjectPtr<class UTexture2D>& GetInformationImage() const { return InformationImage; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELS2DWidgetProperty WidgetProperty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true, ToolTip = "Multi-line text supported"))
	FString InformationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTexture2D> InformationImage;
};

//#include "Level/LSInformationData.h"