// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;


public class Racing_Portfolio : ModuleRules
{
	public Racing_Portfolio(ReadOnlyTargetRules Target) : base(Target)
	{

		PublicIncludePaths.Add(ModuleDirectory);
		PrivateIncludePaths.Add(ModuleDirectory);

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[] { Path.Combine(EngineDirectory, "Source/Runtime/Renderer/Private") });
        PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"ChaosVehicles",
			// Rendering dependencies
            "Renderer",
            "RenderCore",
            "RHI",
            "RHICore",
            "D3D12RHI",
            // OpenCV dependencies
            "OpenCV",
            "OpenCVHelper",
			"NNE",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
