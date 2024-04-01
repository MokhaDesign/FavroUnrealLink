// Copyright Epic Games, Inc. All Rights Reserved.

#include "F2UEStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/StyleColors.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FF2UEStyle::StyleInstance = nullptr;

void FF2UEStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FF2UEStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FF2UEStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("F2UEStyle"));
	return StyleSetName;
}


const FSlateBrush* FF2UEStyle::GetBrush(FName PropertyName, const ANSICHAR* Specifier)
{
	return Get().GetBrush(PropertyName, Specifier);
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FF2UEStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("F2UEStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("F2UE")->GetBaseDir() / TEXT("Resources"));

	Style->Set("F2UE.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("FavroLogo"), Icon20x20));
	Style->Set("Favro.FavroLogo", new IMAGE_BRUSH_SVG(TEXT("FavroLogo"), Icon20x20));
	Style->Set("Favro.Collections", new IMAGE_BRUSH_SVG(TEXT("Collections"), Icon16x16));
	Style->Set("Favro.Settings", new IMAGE_BRUSH_SVG(TEXT("Settings"), Icon16x16));
	Style->Set("Favro.Dashboard", new IMAGE_BRUSH_SVG(TEXT("Dashboard"), Icon16x16));

	Style->Set("Favro.LeftPane", new IMAGE_BRUSH_SVG(TEXT("LeftPane"), Icon16x16));
	Style->Set("Favro.RightPane", new IMAGE_BRUSH_SVG(TEXT("RightPane"), Icon16x16));

	Style->Set("Favro.AddTask", new IMAGE_BRUSH_SVG(TEXT("AddTask"), Icon16x16));
	Style->Set("Favro.AddBugReport", new IMAGE_BRUSH_SVG(TEXT("AddBugReport"), Icon16x16));

	Style->Set("Favro.Users", new IMAGE_BRUSH_SVG(TEXT("Users"), Icon16x16));
	Style->Set("Favro.Organization", new IMAGE_BRUSH_SVG(TEXT("Organization"), Icon16x16));
	Style->Set("Favro.Collection", new IMAGE_BRUSH_SVG(TEXT("Collection"), Icon16x16));
	Style->Set("Favro.Public", new IMAGE_BRUSH_SVG(TEXT("Public"), Icon16x16));
	Style->Set("Favro.Private", new IMAGE_BRUSH_SVG(TEXT("Private"), Icon16x16));
	Style->Set("Favro.Global", new IMAGE_BRUSH_SVG(TEXT("Global"), Icon16x16));

	Style->Set("Favro.EyeOff", new IMAGE_BRUSH_SVG(TEXT("EyeOff"), Icon16x16));
	Style->Set("Favro.EyeOn", new IMAGE_BRUSH_SVG(TEXT("EyeOn"), Icon16x16));

	Style->Set("Favro.RefreshEnabled", new IMAGE_BRUSH_SVG(TEXT("RefreshEnabled"), Icon16x16));
	Style->Set("Favro.RefreshDisabled", new IMAGE_BRUSH_SVG(TEXT("RefreshDisabled"), Icon16x16));

	Style->Set("Favro.CardBoard", new IMAGE_BRUSH_SVG(TEXT("CardBoard"), Icon16x16));
	Style->Set("Favro.ArrowOpen", new IMAGE_BRUSH_SVG(TEXT("ArrowOpen"), Icon16x16));
	Style->Set("Favro.ArrowClosed", new IMAGE_BRUSH_SVG(TEXT("ArrowClosed"), Icon16x16));

	Style->Set("Favro.CardDescription", new IMAGE_BRUSH_SVG(TEXT("CardDescription"), Icon16x16));

	Style->Set("Favro.RoundedBackground", new FSlateRoundedBoxBrush(FFavroColors::Recessed, 4.f));
	Style->Set("Favro.RoundedBackgroundToggle",
	           new FSlateRoundedBoxBrush(FFavroColors::Recessed, 6.f, FFavroColors::Secondary, 1.f));

	Style->Set("Favro.CardBackground", new FSlateRoundedBoxBrush(FFavroColors::Secondary, 6.f));

	// FStyleColors::Transparent, 4.0f, FStyleColors::Warning, 1.0f

	const FCheckBoxStyle ExpanderStyle =
		FCheckBoxStyle()
		.SetUncheckedImage(
			IMAGE_BRUSH_SVG(TEXT("ArrowClosed"), Icon20x20))
		.SetUncheckedHoveredImage(
			IMAGE_BRUSH_SVG(TEXT("ArrowClosed"), Icon20x20, FStyleColors::White))
		.SetUncheckedPressedImage(
			IMAGE_BRUSH_SVG(TEXT("ArrowClosed"), Icon20x20))
		.SetCheckedImage(
			IMAGE_BRUSH_SVG(TEXT("ArrowOpen"), Icon20x20, FStyleColors::White))
		.SetCheckedHoveredImage(
			IMAGE_BRUSH_SVG(TEXT("ArrowOpen"), Icon20x20))
		.SetCheckedPressedImage(
			IMAGE_BRUSH_SVG(TEXT("ArrowOpen"), Icon20x20, FStyleColors::White));
	Style->Set("Favro.ExpanderStyle", ExpanderStyle);

	return Style;
}

void FF2UEStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FF2UEStyle::Get()
{
	return *StyleInstance;
}


FSlateColor FF2UEStyle::ConvertToSlateColor(const FString& String)
{
	if (String == "lightgreen")
	{
		return FFavroColors::LightGreen;
	}
	if (String == "brown")
	{
		return FFavroColors::Brown;
	}
	if (String == "purple")
	{
		return FFavroColors::Purple;
	}
	if (String == "orange")
	{
		return FFavroColors::Orange;
	}
	if (String == "yellow")
	{
		return FFavroColors::Yellow;
	}
	if (String == "gray")
	{
		return FFavroColors::Gray;
	}
	if (String == "red")
	{
		return FFavroColors::Red;
	}
	if (String == "cyan")
	{
		return FFavroColors::Cyan;
	}
	if (String == "green")
	{
		return FFavroColors::Green;
	}

	return FFavroColors::Blue;
}
