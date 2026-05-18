// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Team26 : ModuleRules
{
	public Team26(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"ChaosVehicles", 
			"PhysicsCore",
			"Landscape" ,
			"UMG",
			"GeoReferencing"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
		});
	}
}
