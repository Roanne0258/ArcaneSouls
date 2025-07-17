// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ArcaneSouls : ModuleRules
{
	public ArcaneSouls(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		/* ────────────── Runtime Modules ────────────── */
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",

			/* Combat & FX */
			"GameplayAbilities",     // GAS
			"GameplayTags",
			"GameplayTasks",
			"Niagara",

			/* UI */
			"UMG",
			"Slate",
			"SlateCore"
		});

		/* ────────────── Editor-only Modules ────────────── */
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new[]
			{
				"UnrealEd",            // 에디터 유틸리티 위젯
				"Blutility",           // Editor Utility
				"EditorSubsystem",
				"PropertyEditor"       // Details 패널 커스텀
			});
		}

		/* ────────────── Precompiled Header 옵션 ────────────── */
		// 필요 시: MinFiles = true; Unity = false;
	}
}
