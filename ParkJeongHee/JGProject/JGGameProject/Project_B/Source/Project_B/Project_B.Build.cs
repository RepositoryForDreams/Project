// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_B : ModuleRules
{
	public Project_B(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
