// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "F2UEStyle.h"

class FF2UECommandInfo
{
public:
	FF2UECommandInfo(const FName InExtensionHook, const FString& InLabelOverride, const FString& InToolTipOverride,
	                 const FSlateIcon& InIcon)
	{
		ExtensionHook = InExtensionHook;
		LabelOverride = FText::FromString(InLabelOverride);
		ToolTipOverride = FText::FromString(InToolTipOverride);
		Icon = InIcon;
	}

public:
	FName ExtensionHook;
	FText LabelOverride;
	FText ToolTipOverride;
	FSlateIcon Icon;
};

class FF2UECommands : public TCommands<FF2UECommands>
{
public:
	FF2UECommands()
		: TCommands<FF2UECommands>(TEXT("FavroLink"), NSLOCTEXT("Contexts", "F2UE", "Favro Link Commands"), NAME_None,
		                           FF2UEStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
	static void AddMenuEntry(FMenuBuilder& InMenuBuilder,
	                         const TSharedPtr<FF2UECommandInfo>& InCommandInfo,
	                         const FUIAction& InUIAction);
	static void AddMenuEntry(FToolBarBuilder& InToolbarBuilder, const TSharedPtr<FF2UECommandInfo>& InCommandInfo,
	                         const FUIAction& InUIAction);

	TSharedPtr<FF2UECommandInfo> AddTask;
	TSharedPtr<FF2UECommandInfo> AddBugReport;

	TSharedPtr<FF2UECommandInfo> ToggleLeftPane;
	TSharedPtr<FF2UECommandInfo> ToggleRightPane;

	TSharedPtr<FF2UECommandInfo> RefreshCollections;
	TSharedPtr<FF2UECommandInfo> RefreshOrganizations;
};
