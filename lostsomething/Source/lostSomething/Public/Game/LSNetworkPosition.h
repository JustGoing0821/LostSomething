#pragma once

#include "CoreMinimal.h"
#include "LSNetworkPosition.generated.h"

UENUM(BlueprintType)
enum class ELSNetworkPosition : uint8
{
	None = 0,
	Server,
	Client
};

//#include "Game/LSNetworkPosition.h"