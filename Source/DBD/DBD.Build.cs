// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DBD : ModuleRules
{
	public DBD(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(new string[] { "DBD" });

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
