using UnrealBuildTool;

public class AnotherWorkingTitleEditor : ModuleRules
{
    public AnotherWorkingTitleEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "UnrealEd",
                "ComponentVisualizers",
                "AnotherWorkingTitle"
            }
        );
    }
}