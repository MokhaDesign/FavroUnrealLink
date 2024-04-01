// Copyright Epic Games, Inc. All Rights Reserved.

#include "F2UECommands.h"

#define LOCTEXT_NAMESPACE "FF2UEModule"

void FF2UECommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Favro Link", "Bring up Favro Link window", EUserInterfaceActionType::Button,
	           FInputChord());

	AddTask = MakeShared<FF2UECommandInfo>(
		"FF2UEToolbar",
		"Add New Task",
		"Add new task",
		FSlateIcon(FName("F2UEStyle"), "Favro.AddTask")
	);

	AddBugReport = MakeShared<FF2UECommandInfo>(
		"FF2UEToolbar",
		"Add Bug Report",
		"Add a new bug report",
		FSlateIcon(FName("F2UEStyle"), "Favro.AddBugReport")
	);

	ToggleLeftPane = MakeShared<FF2UECommandInfo>(
		"FF2UEToolbar",
		"Toggle Left Pane",
		"Toggle left pane visibility",
		FSlateIcon(FName("F2UEStyle"), "Favro.LeftPane")
	);

	ToggleRightPane = MakeShared<FF2UECommandInfo>(
		"FF2UEToolbar",
		"Toggle Right Pane",
		"Toggle right pane visibility",
		FSlateIcon(FName("F2UEStyle"), "Favro.RightPane")
	);

	RefreshOrganizations = MakeShared<FF2UECommandInfo>(
		"FF2UEToolbar",
		"Refresh Collections",
		"Force a Collection refresh",
		FSlateIcon(FName("F2UEStyle"), "Favro.RefreshCollections")
	);

	RefreshCollections = MakeShared<FF2UECommandInfo>(
		"FF2UEToolbar",
		"Refresh Organizations",
		"Force an Organizations refresh",
		FSlateIcon(FName("F2UEStyle"), "Favro.RefreshOrganizations")
	);
}

void FF2UECommands::AddMenuEntry(FMenuBuilder& InMenuBuilder, const TSharedPtr<FF2UECommandInfo>& InCommandInfo,
                                 const FUIAction& InUIAction)
{
	const FF2UECommandInfo* CommandInfoLocal = InCommandInfo.Get();

	InMenuBuilder.AddMenuEntry(
		CommandInfoLocal->LabelOverride,
		CommandInfoLocal->ToolTipOverride,
		CommandInfoLocal->Icon,
		InUIAction,
		CommandInfoLocal->ExtensionHook,
		EUserInterfaceActionType::Button);
}

void FF2UECommands::AddMenuEntry(FToolBarBuilder& InToolbarBuilder, const TSharedPtr<FF2UECommandInfo>& InCommandInfo,
                                 const FUIAction& InUIAction)
{
	const FF2UECommandInfo* CommandInfoLocal = InCommandInfo.Get();

	InToolbarBuilder.AddToolBarButton(
		InUIAction,
		NAME_None,
		CommandInfoLocal->LabelOverride,
		CommandInfoLocal->ToolTipOverride,
		CommandInfoLocal->Icon);
}


#undef LOCTEXT_NAMESPACE
