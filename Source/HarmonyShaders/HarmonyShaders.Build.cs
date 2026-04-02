// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HarmonyShaders : ModuleRules
{
    public HarmonyShaders(ReadOnlyTargetRules Target) : base(Target)
    {
		bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core"
            });

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "RenderCore",
                "Projects"
            });
    }
}
