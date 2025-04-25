#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSQuestData.generated.h"

USTRUCT(BlueprintType)
struct FLSQuestData : public FTableRowBase
{
    GENERATED_BODY()
    
public:
    FLSQuestData() {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString QuestName; // Äù½ºÆ® ÀÌ¸§

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description; // Äù½ºÆ® ¼³¸í

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELSInteractionEnum CurrentQuestEnum; // Äù½ºÆ® Enum
};
