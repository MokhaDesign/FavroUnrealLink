// Copyright Epic Games, Inc. All Rights Reserved.

#include "F2UE.h"
#include "F2UEStyle.h"
#include "F2UECommands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Slate/FavroLinkView.h"

#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Models/FavroLinkModel.h"

static const FName F2UETabName("F2UE");

#define LOCTEXT_NAMESPACE "FF2UEModule"

void FF2UEModule::StartupModule()
{
	FF2UEStyle::Initialize();
	FF2UEStyle::ReloadTextures();

	FF2UECommands::Register();

	Model = MakeShareable(new FFavroLinkModel());

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FF2UECommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FF2UEModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FF2UEModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		                        F2UETabName,
		                        FOnSpawnTab::CreateRaw(this, &FF2UEModule::OnSpawnPluginTab))
	                        .SetDisplayName(LOCTEXT("FF2UETabTitle", "Favro Link"))
	                        .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
	                        .SetMenuType(ETabSpawnerMenuType::Enabled)
	                        .SetIcon(FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.FavroLogo"));
}

void FF2UEModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);
	FF2UEStyle::Shutdown();

	FF2UECommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(F2UETabName);
}

TSharedRef<SDockTab> FF2UEModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(MajorTab);

	const TSharedPtr<SWidget> TabContent = SNew(SFavroLinkView, DockTab, DockTab->GetParentWindow(), Model);
	DockTab->SetContent(TabContent.ToSharedRef());

	return DockTab;
}

void FF2UEModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(F2UETabName);
}

void FF2UEModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("Developer Tools");
			Section.AddMenuEntryWithCommandList(FF2UECommands::Get().OpenPluginWindow, PluginCommands,
			                                    TAttribute<FText>(), TAttribute<FText>(),
			                                    FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.FavroLogo"));
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FF2UECommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
				Entry.Icon = FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.FavroLogo");
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FF2UEModule, F2UE)
