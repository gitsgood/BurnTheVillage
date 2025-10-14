// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BurnTheVillage : ModuleRules
{
	public BurnTheVillage(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate",
			"Json",
			"JsonUtilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Json",
            "JsonUtilities"
        });

		PublicIncludePaths.AddRange(new string[] {
			"BurnTheVillage",
			"BurnTheVillage/Variant_Strategy",
			"BurnTheVillage/Variant_Strategy/UI",
			"BurnTheVillage/Variant_TwinStick",
			"BurnTheVillage/Variant_TwinStick/AI",
			"BurnTheVillage/Variant_TwinStick/Gameplay",
			"BurnTheVillage/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
