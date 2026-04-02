// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Harmony : ModuleRules
{
    public Harmony(ReadOnlyTargetRules Target) : base(Target)
    {
		bUsePrecompiled = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            });

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "DeveloperSettings",
                "RHI",
                "Renderer",
                "RenderCore",
                "Projects"
            });

        PrivateIncludePaths.AddRange(
            new[]
            {
                Path.Combine(EngineDirectory, "Source/Runtime/Renderer/Internal"),
                Path.Combine(EngineDirectory, "Source/Runtime/Renderer/Private")
            });
    }
}
