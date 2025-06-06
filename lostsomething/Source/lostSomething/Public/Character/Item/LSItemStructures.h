// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "LSItemStructures.generated.h"

// AMasterItem 클래스 전방 선언 (Item_Class 필드용)
class AMasterItem;

// Item_Nature enum (블루프린트의 드롭다운과 동일)
UENUM(BlueprintType)
enum class EItemNature : uint8
{
    IsConsumable    UMETA(DisplayName = "IsConsumable"),
    IsEquipment     UMETA(DisplayName = "IsEquipment"),
    IsMaterial      UMETA(DisplayName = "IsMaterial"),
    IsKeyItem       UMETA(DisplayName = "IsKeyItem")
};


USTRUCT(BlueprintType)
struct LOSTSOMETHING_API FItemDetails : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 아이템 이름 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
    FName Item_Name = NAME_None;

    // 아이템 아이콘 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
    TSoftObjectPtr<UTexture2D> Item_Icon = nullptr;

    // 아이템 성질 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
    EItemNature Item_Nature = EItemNature::IsConsumable;

    // 빈 슬롯 여부 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
    bool IsEmpty = true;

    // 아이템 클래스 레퍼런스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
    TSubclassOf<AMasterItem> Item_Class = nullptr;

    // 기본 생성자 
    FItemDetails()
    {
        Item_Name = NAME_None;
        Item_Icon = nullptr;
        Item_Nature = EItemNature::IsConsumable;
        IsEmpty = true;
        Item_Class = nullptr;
    }
};