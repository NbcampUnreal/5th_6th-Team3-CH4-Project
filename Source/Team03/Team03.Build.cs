// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class Team03 : ModuleRules
{
	public Team03(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			// Initial Dependencies
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" , "UMG" , "Slate", "SlateCore",

			// Online Subsystem Dependencies
            "OnlineSubsystem", "OnlineSubsystemUtils",

			// Engine Settings for GameMapsSettings
			"EngineSettings",
            
            // DLSS (Disabled for now)
            // "DLSSBlueprint"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { "SelectSkeletal", });

		PublicIncludePaths.AddRange(new string[] { "Team03" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
