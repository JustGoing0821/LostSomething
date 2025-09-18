#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "LSInteractionEnum.generated.h"

UENUM(BlueprintType)
enum class ELSInteractionEnum : uint8
{
	Quest0 = 0 UMETA(DisplayName = "Quest 0"),
	Quest1 UMETA(DisplayName = "Quest 1"),
	Quest2 UMETA(DisplayName = "Quest 2"),
	Quest3 UMETA(DisplayName = "Quest 3"),
	Quest4 UMETA(DisplayName = "Quest 4"),
	Quest5 UMETA(DisplayName = "Quest 5"),
	Quest6 UMETA(DisplayName = "Quest 6"),
	Quest7 UMETA(DisplayName = "Quest 7"),
	Quest8 UMETA(DisplayName = "Quest 8"),
	Quest9 UMETA(DisplayName = "Quest 9"),
	Quest10 UMETA(DisplayName = "Quest 10"),
	Quest11 UMETA(DisplayName = "Quest 11"),
	Quest12 UMETA(DisplayName = "Quest 12")
};
