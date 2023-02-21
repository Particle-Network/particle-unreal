// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ParticleSDK : ModuleRules
{
	public ParticleSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"WebBrowser",
				"Slate",
				"SlateCore",
				"UMG",
				"Engine",
				"HTTP",
				"Json",
				"JsonUtilities",
				"Projects",
			}
		);

		if (Target.bBuildEditor == true)
		{
			//@TODO: UnrealEd Needed for the triangulation code used for sprites (but only in editor mode)
			//@TOOD: Try to move the code dependent on the triangulation code to the editor-only module
			PrivateIncludePathModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
				}
			);
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"EditorFramework",
					"UnrealEd",
				}
			);
		}

		if (Target.bBuildEditor || Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
		{
			// WebBrowserTexture required for cooking Android
			PrivateIncludePathModuleNames.AddRange(
				new string[]
				{
					"WebBrowserTexture",
				}
			);
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"WebBrowserTexture",
				}
			);
		}
	}
}