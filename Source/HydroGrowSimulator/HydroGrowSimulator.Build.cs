using UnrealBuildTool;

public class HydroGrowSimulator : ModuleRules
{
	public HydroGrowSimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"ToolMenus"
		});
	}
}