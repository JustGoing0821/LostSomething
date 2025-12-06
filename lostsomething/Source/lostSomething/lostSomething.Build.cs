// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class lostSomething : ModuleRules
{
	public lostSomething(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "lostSomething" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTasks", "OnlineSubsystem","OnlineSubsystemSteam", "Niagara", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "Media", "MediaAssets" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });

    }


    // Uncomment if you are using Slate UI
    // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

    // Uncomment if you are using online features
    // PrivateDependencyModuleNames.Add("OnlineSubsystem");

    // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
}

