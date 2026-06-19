using UnrealBuildTool;

public class LastBell : ModuleRules
{
    public LastBell(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "SlateCore",
            "Slate",
            "PhysicsCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
