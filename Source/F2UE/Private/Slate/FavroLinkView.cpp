// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/FavroLinkView.h"

#include "F2UECommands.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"
#include "Slate/Tabs/FavroCollectionsList.h"
#include "Slate/Tabs/FavroDashboard.h"
#include "Slate/Tabs/FavroLinkSettings.h"
#include "Slate/Widgets/FavroProfileCard.h"


#define LOCTEXT_NAMESPACE "SFavroLinkView"

static const FName CollectionsTabId("Collections");
static const FName SettingsTabId("Settings");
static const FName DashboardTabId("Dashboard");

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SFavroLinkView::SFavroLinkView()
	: CommandList(MakeShareable(new FUICommandList))
{
}

void SFavroLinkView::Construct(const FArguments& InArgs,
                               const TSharedRef<SDockTab>& MajorTab,
                               const TSharedPtr<SWindow>& Window, const TSharedPtr<FFavroLinkModel> InModel)
{
	Model = InModel;
	
	FSlimHorizontalToolBarBuilder ToolbarBuilder(CommandList, FMultiBoxCustomization::None);

	RegisterToolbarEntries(ToolbarBuilder);

	RegisterTabs(MajorTab);
	const TSharedRef<FTabManager::FLayout> Layout = MakeLayout();
	const TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	MenuBarBuilder.SetExtendersEnabled(true);

	RegisterMenus(MenuBarBuilder);

	TabManager->SetAllowWindowMenuBar(true);
	TabManager->SetMenuMultiBox(MenuBarBuilder.GetMultiBox(), MenuBarBuilder.MakeWidget());
	VerticalBox->AddSlot()[ToolbarBuilder.MakeWidget()].AutoHeight();
	VerticalBox->AddSlot()[TabManager->RestoreFrom(Layout, Window).ToSharedRef()].FillHeight(1.f).Padding(4.f);

	ChildSlot
	[
		VerticalBox.ToSharedRef()
	];

	Model->OnCollectionChanged.AddSP(this, &SFavroLinkView::HandleCollectionChanged);
}

void SFavroLinkView::RegisterToolbarEntries(FSlimHorizontalToolBarBuilder& ToolbarBuilder) const
{
	ToolbarBuilder.AddWidget(SNew(SFavroProfileCard, Model.ToSharedRef()));
	ToolbarBuilder.AddSeparator();

	// FF2UECommands::AddMenuEntry(ToolbarBuilder,
	//                             FF2UECommands::Get().AddTask,
	//                             FUIAction(FExecuteAction::CreateStatic(
	// 	                            &SFavroLinkView::HandleAddNewTask)));

	FF2UECommands::AddMenuEntry(ToolbarBuilder,
	                            FF2UECommands::Get().AddBugReport,
	                            FUIAction(FExecuteAction::CreateStatic(
		                            &SFavroLinkView::HandleAddBugReport)));
}


void SFavroLinkView::RegisterMenus(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("ActionsMenuLabel", "Actions"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SFavroLinkView::FillActionsMenu),
		"FavroLink.Actions"
	);

	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SFavroLinkView::FillWindowMenu, TabManager),
		"FavroLink.Window"
	);
}

void SFavroLinkView::RegisterTabs(const TSharedRef<SDockTab>& MajorTab)
{
	TabManager = FGlobalTabmanager::Get()->NewTabManager(MajorTab);
	{
		TSharedRef<FWorkspaceItem> MenuGroup = TabManager->AddLocalWorkspaceMenuCategory(
			LOCTEXT("FavroLinkGroupName", "Favro Link"));

		TabManager->RegisterTabSpawner(CollectionsTabId,
		                               FOnSpawnTab::CreateRaw(this, &SFavroLinkView::HandleSpawnTab,
		                                                      CollectionsTabId))
		          .SetDisplayName(LOCTEXT("TaskCollectionsTabTitle", "Collections"))
		          .SetGroup(MenuGroup)
		          .SetIcon(FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.Collections"))
		          .SetCanSidebarTab(true);

		TabManager->RegisterTabSpawner(SettingsTabId,
		                               FOnSpawnTab::CreateRaw(this, &SFavroLinkView::HandleSpawnTab, SettingsTabId))
		          .SetDisplayName(LOCTEXT("SettingsTabTitle", "Settings"))
		          .SetGroup(MenuGroup)
		          .SetIcon(FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.Settings"))
		          .SetCanSidebarTab(false);

		TabManager->RegisterTabSpawner(DashboardTabId,
		                               FOnSpawnTab::CreateRaw(this, &SFavroLinkView::HandleSpawnTab, DashboardTabId))
		          .SetDisplayName(LOCTEXT("DashboardTabTitle", "Dashboard"))
		          .SetGroup(MenuGroup)
		          .SetIcon(FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.Dashboard"))
		          .SetCanSidebarTab(false);
	}
}

TSharedRef<FTabManager::FLayout> SFavroLinkView::MakeLayout() const
{
	return FTabManager::NewLayout("FavroLinkLayout_v0.1")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.25f)
				->Split(
					FTabManager::NewStack()
					->AddTab(CollectionsTabId, ETabState::OpenedTab)
				)
			)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.75f)
				->Split(
					FTabManager::NewStack()
					->AddTab(DashboardTabId, ETabState::OpenedTab)
					->AddTab(SettingsTabId, ETabState::OpenedTab)
					->SetForegroundTab(DashboardTabId)
				)
			)
		);
}

TSharedRef<SDockTab> SFavroLinkView::HandleSpawnTab(const FSpawnTabArgs& Args, FName TabId)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
	.ShouldAutosize(false)
	.TabRole(PanelTab)
	.CanEverClose(TabId != DashboardTabId);
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	if (TabId == SettingsTabId)
	{
		TabWidget = SNew(SFavroLinkSettings, Model.ToSharedRef());
	}

	else if (TabId == CollectionsTabId)
	{
		TabWidget = SNew(SFavroCollectionsList, Model.ToSharedRef());
	}

	else if (TabId == DashboardTabId)
	{
		TabWidget = SNew(SFavroDashboard, DockTab, Model.ToSharedRef(), CommandList);
	}

	DockTab->SetContent(TabWidget->AsShared());

	return DockTab;
}

void SFavroLinkView::FillWindowMenu(FMenuBuilder& MenuBuilder, TSharedPtr<FTabManager, ESPMode::ThreadSafe> TabManager)
{
	if (!TabManager.IsValid())
	{
		return;
	}

	TabManager->PopulateTabSpawnerMenu(MenuBuilder, TabManager->GetLocalWorkspaceMenuRoot());
}

void SFavroLinkView::FillActionsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("FavroActionsMenu", LOCTEXT("Task Actions", "Task Actions"));
	{
		FF2UECommands::AddMenuEntry(MenuBuilder,
		                            FF2UECommands::Get().AddBugReport,
		                            FUIAction(FExecuteAction::CreateStatic(&SFavroLinkView::HandleAddBugReport)));

		FF2UECommands::AddMenuEntry(MenuBuilder, FF2UECommands::Get().AddTask,
		                            FUIAction(FExecuteAction::CreateStatic(&SFavroLinkView::HandleAddNewTask)));
	}
	MenuBuilder.EndSection();
}

void SFavroLinkView::HandleAddNewTask()
{
}

void SFavroLinkView::HandleAddBugReport()
{
}

void SFavroLinkView::HandleCollectionChanged(const FFavroCollection& FavroCollection)
{
	if (const TSharedPtr<SDockTab> DockTab = TabManager->FindExistingLiveTab(DashboardTabId); DockTab.IsValid())
	{
		TabManager->DrawAttention(DockTab.ToSharedRef());
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
