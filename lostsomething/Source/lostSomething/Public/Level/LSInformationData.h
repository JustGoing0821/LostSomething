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
	FORCEINLINE const FString& GetInformationData() const { return InformationData; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELS2DWidgetProperty WidgetProperty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true, ToolTip = "Multi-line text supported"))
	FString InformationData;
};

//#include "Level/LSInformationData.h"