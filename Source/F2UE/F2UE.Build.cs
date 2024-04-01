// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class F2UE : ModuleRules
{
	public F2UE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new[]
			{
				EngineDirectory + "/Source/Runtime/Slate/Public/"
				// ... add public include paths required here ...
			}
		);

		PrivateIncludePaths.AddRange(
			new[]
			{
				EngineDirectory + "/Source/Runtime/Slate/Private/"
			}
		);


		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"DeveloperSettings",
				"PropertyEditor",
				"HTTP",
				"Json",
				"JsonUtilities",
				"Slate",
				"SlateCore",
				"ImageDownload"
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"WorkspaceMenuStructure",
				"DeveloperSettings",
				"PropertyEditor"
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}