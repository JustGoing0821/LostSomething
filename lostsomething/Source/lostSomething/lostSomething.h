// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client)? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : (GetNetMode() == ENetMode::NM_Standalone)? TEXT("STANDALONE") : TEXT("SERVER"))
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define LOG_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define LOG_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define LOG_WIDGET_NETMODEINFO (GetWorld() ? ((GetWorld()->GetNetMode() == ENetMode::NM_Client)? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : (GetWorld()->GetNetMode() == ENetMode::NM_Standalone)? TEXT("STANDALONE") : TEXT("SERVER")) : TEXT("UNKNOWN"))

#define LS_LOG(CatName, Verbosity, Format, ...) UE_LOG(CatName, Verbosity, TEXT("[%s] %s : %s"), LOG_NETMODEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
/******Log format (UObject)*********

#include "lostSomething.h"

LS_LOG(LogLS, [Log,Warning,Error], ... ... );

**************************/

#define LS_SUBLOG(CatName, Verbosity, Format, ...) UE_LOG(CatName, Verbosity, TEXT("[%s][%s/%s] %s : %s"), LOG_NETMODEINFO, LOG_SUBLOCALROLEINFO, LOG_SUBREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
/******Log format (USubObject-forActorComponent)*********

#include "lostSomething.h"

LS_SUBLOG(LogLS, [Log,Warning,Error], ... ... );

**************************/

#define LS_WDGLOG(CatName, Verbosity, Format, ...) UE_LOG(CatName, Verbosity, TEXT("[%s] %s : %s"), LOG_WIDGET_NETMODEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogLS, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogLSls, Log, All);