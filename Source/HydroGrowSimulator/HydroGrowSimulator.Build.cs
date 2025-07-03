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
			"EnhancedInput",
			"ProceduralMeshComponent",
			"GameplayTags",
			"NetCore",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"ToolMenus",
			"RenderCore",
			"RHI",
			"NavigationSystem",
			"AIModule"
		});
	}
}