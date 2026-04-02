using UnrealBuildTool;

public class HarmonyEditor : ModuleRules
{
    public HarmonyEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "AssetTools",
                "ContentBrowser",
                "CoreUObject",
                "Engine",
                "FileUtilities",
                "Harmony",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "UnrealEd"
            });

        AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");
    }
}
