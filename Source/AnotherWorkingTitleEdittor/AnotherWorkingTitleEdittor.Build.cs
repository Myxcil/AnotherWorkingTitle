using UnrealBuildTool;

public class AnotherWorkingTitleEdittor : ModuleRules
{
    public AnotherWorkingTitleEdittor(ReadOnlyTargetRules Target) : base(Target)
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