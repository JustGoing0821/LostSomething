#pragma once

#include "CoreMinimal.h"
#include "LS2DWidgetProperty.generated.h"

UENUM(BlueprintType)
enum class ELS2DWidgetProperty : uint8
{
	None = 0,
	Information,
	Drag
};

//#include "Puzzle/UI/LS2DWidgetProperty.h"