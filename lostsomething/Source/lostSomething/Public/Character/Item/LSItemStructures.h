// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "LSItemStructures.generated.h"

// 아이템 타입 열거형
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None			UMETA(DisplayName = "None"),
	Consumable		UMETA(DisplayName = "Consumable"),
	Equipment		UMETA(DisplayName = "Equipment"),
	Material		UMETA(DisplayName = "Material"),
	KeyItem			UMETA(DisplayName = "Key Item"),
	Currency		UMETA(DisplayName = "Currency")
};

// 아이템 희귀도 열거형
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common		UMETA(DisplayName = "Common"),
	Uncommon	UMETA(DisplayName = "Uncommon"),
	Rare		UMETA(DisplayName = "Rare"),
	Epic		UMETA(DisplayName = "Epic"),
	Legendary	UMETA(DisplayName = "Legendary")
};

// 아이템 타입 구조체 (블루프린트의 ItemType에 해당)
USTRUCT(BlueprintType)
struct LOSTSOMETHING_API FItemType : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 타입 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	int32 TypeID = 0;

	// 타입 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	FText TypeName = FText::FromString("Default Type");

	// 카테고리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	EItemCategory Category = EItemCategory::None;

	// 타입 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	FText Description = FText::FromString("Default Description");

	// 기본 아이콘 (타입별 공통 아이콘)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	TSoftObjectPtr<UTexture2D> DefaultIcon = nullptr;

	// 기본 최대 스택 사이즈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	int32 DefaultMaxStackSize = 1;

	// 판매 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	bool bCanSell = true;

	// 드롭 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	bool bCanDrop = true;

	// 기본 생성자
	FItemType()
	{
		TypeID = 0;
		TypeName = FText::FromString("Default Type");
		Category = EItemCategory::None;
		Description = FText::FromString("Default Description");
		DefaultIcon = nullptr;
		DefaultMaxStackSize = 1;
		bCanSell = true;
		bCanDrop = true;
	}
};

// 아이템 상세 정보 구조체 (블루프린트의 ItemDetails에 해당)
USTRUCT(BlueprintType)
struct LOSTSOMETHING_API FItemDetails : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 아이템 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	int32 ItemID = 0;

	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	FText ItemName = FText::FromString("Default Item");

	// 아이템 타입 참조 (ItemType 테이블의 RowName)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	FName ItemTypeRowName = NAME_None;

	// 아이템 희귀도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	EItemRarity Rarity = EItemRarity::Common;

	// 아이템 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	FText Description = FText::FromString("Default item description");

	// 아이템 전용 아이콘 (없으면 타입의 기본 아이콘 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;

	// 아이템별 최대 스택 사이즈 (0이면 타입의 기본값 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	int32 MaxStackSize = 0;

	// 아이템 가치 (판매/구매 가격)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	int32 Value = 0;

	// 무게 (인벤토리 무게 제한이 있는 경우)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	float Weight = 0.0f;

	// 사용 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	bool bIsUsable = false;

	// 장비 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	bool bIsEquippable = false;

	// 퀘스트 아이템 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Details")
	bool bIsQuestItem = false;

	// 기본 생성자
	FItemDetails()
	{
		ItemID = 0;
		ItemName = FText::FromString("Default Item");
		ItemTypeRowName = NAME_None;
		Rarity = EItemRarity::Common;
		Description = FText::FromString("Default item description");
		ItemIcon = nullptr;
		MaxStackSize = 0;
		Value = 0;
		Weight = 0.0f;
		bIsUsable = false;
		bIsEquippable = false;
		bIsQuestItem = false;
	}
};