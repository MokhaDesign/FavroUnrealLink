// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/StyleColors.h"

/**  */
class FF2UEStyle
{
public:
	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();
	static FSlateColor ConvertToSlateColor(const FString& String);

	static FName GetStyleSetName();
	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = nullptr);

private:
	static TSharedRef<class FSlateStyleSet> Create();

private:
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};

struct FFavroColors
{
	inline static const FSlateColor Warning = FStyleColors::Warning;
	inline static const FSlateColor Recessed = FStyleColors::Recessed;
	inline static const FSlateColor Secondary = FStyleColors::Secondary;

	inline static const FSlateColor Blue = FStyleColors::AccentBlue;
	inline static const FSlateColor Green = FStyleColors::AccentGreen;
	inline static const FSlateColor Brown = FStyleColors::AccentBrown;
	inline static const FSlateColor Purple = FStyleColors::AccentPurple;
	inline static const FSlateColor Orange = FStyleColors::AccentOrange;
	inline static const FSlateColor Yellow = FStyleColors::AccentYellow;
	inline static const FSlateColor Gray = FStyleColors::AccentGray;
	inline static const FSlateColor Red = FStyleColors::AccentRed;
	inline static const FSlateColor Cyan = FStyleColors::SelectInactive;
	inline static const FSlateColor LightGreen = FStyleColors::AccentGreen;

	inline static const FSlateColor Transparent = FStyleColors::Transparent;
};
