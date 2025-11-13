// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LSInteractionEnum.h"
#include "LSInteractionScriptData.generated.h"

USTRUCT(BlueprintType)
struct FScriptArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	TArray<FString> Scripts;
};

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ULSInteractionScriptData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("LSScriptData", GetFName());
	}

	FORCEINLINE const TArray<FString>& GetInteractionScripts(ELSInteractionEnum InSequence) const
	{
		if (InteractiveScriptsData.Contains(InSequence))
		{
			return InteractiveScriptsData[InSequence].Scripts;
		}
		else
		{
			static const TArray<FString> EmptyArray;
			return EmptyArray;
		}
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Scripts)
	TMap<ELSInteractionEnum, FScriptArrayWrapper> InteractiveScriptsData;
};
