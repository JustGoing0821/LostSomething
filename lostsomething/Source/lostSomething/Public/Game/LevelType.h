#pragma once

#include "CoreMinimal.h"
#include "LevelType.generated.h"

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	NewStart,
	Tutorial,
	Stage1,
	Stage2,
	Stage3,
};
