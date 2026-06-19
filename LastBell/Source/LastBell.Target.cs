using UnrealBuildTool;
using System.Collections.Generic;

public class LastBellTarget : TargetRules
{
    public LastBellTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5Latest;
        ExtraModuleNames.Add("LastBell");
    }
}
