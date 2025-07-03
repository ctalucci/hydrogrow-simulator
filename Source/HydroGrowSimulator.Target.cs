using UnrealBuildTool;
using System.Collections.Generic;

public class HydroGrowSimulatorTarget : TargetRules
{
	public HydroGrowSimulatorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("HydroGrowSimulator");
	}
}