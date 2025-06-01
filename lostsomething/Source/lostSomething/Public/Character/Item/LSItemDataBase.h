// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LSItemDataBase.generated.h"


class AItem;

USTRUCT(BlueprintType)
struct LOSTSOMETHING_API FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AItem> Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* Icon;

	bool operator==(const FItemData& Other) const
	{
		return ItemName == Other.ItemName;
	}
};

UCLASS()
class LOSTSOMETHING_API UItemUIObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FItemData ItemData;
};

// 실제로 에셋에 저장될 아이템 목록
UCLASS()
class LOSTSOMETHING_API ULSItemDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<FItemData> Items;
};