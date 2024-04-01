// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Tabs/FavroDashboard.h"

#include "F2UECommands.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"
#include "Slate/Private/Framework/Docking/SDockingTabStack.h"
#include "Slate/Tabs/FavroBoardsList.h"
#include "Slate/Widgets/FavroCollectionCard.h"
#include "Slate/Widgets/FavroPaneDrawer.h"

#define LOCTEXT_NAMESPACE "SFavroLinkDashboard"

static const FName BacklogTabId("Backlog");
static const FName BoardTabId("Board");

#define LEFT_PANE true
#define RIGHT_PANE false

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroDashboard::Construct(const FArguments& InArgs,
                                const TSharedRef<SDockTab>& InDockTab,
                                const TSharedRef<FFavroLinkModel>& InModel,
                                const TSharedRef<FUICommandList>& InCommandList)
{
	Model = InModel;
	CommandList = InCommandList;
	DockTab = InDockTab;

	FSlimHorizontalToolBarBuilder ToolbarBuilder(CommandList, FMultiBoxCustomization::None);
	RegisterToolbarEntries(ToolbarBuilder);

	RegisterTabs();

	const TSharedRef<FTabManager::FLayout> Layout = MakeLayout();
	const TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	const TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(DockTab->AsShared());

	const TSharedRef<SSplitter> ContentPanel = SNew(SSplitter)
		.Orientation(Orient_Horizontal);

	TSharedRef<FFavroPaneDrawerArgs> LeftPaneArgs = MakeShared<FFavroPaneDrawerArgs>(EFavroPaneType::Backlog);
	TSharedRef<FFavroPaneDrawerArgs> RightPaneArgs = MakeShared<FFavroPaneDrawerArgs>(EFavroPaneType::Board);


	ContentPanel->AddSlot()
	            .Value(0.05f)
	            .Resizable(false)
	            .SizeRule(SSplitter::ESizeRule::SizeToContent)
	[
		SNew(SFavroPaneDrawer, LeftPaneArgs, SharedThis(this))
		.Visibility(this, &SFavroDashboard::IsPaneDrawerVisible, LEFT_PANE)
	];

	ContentPanel->AddSlot()
	            .Value(0.35f)
	[
		TabManager->RestoreFrom(Layout, ParentWindow)->AsShared()
	];

	ContentPanel->AddSlot()
	            .Value(0.05f)
	            .Resizable(false)
	            .SizeRule(SSplitter::ESizeRule::SizeToContent)
	[
		SNew(SFavroPaneDrawer, RightPaneArgs, SharedThis(this))
		.Visibility(this, &SFavroDashboard::IsPaneDrawerVisible, RIGHT_PANE)
	];


	VerticalBox->AddSlot()[ToolbarBuilder.MakeWidget()].AutoHeight();
	VerticalBox->AddSlot()[
		SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Background"))
		[
			ContentPanel
		].Padding(0.f, 4.f, 0.f, 0.f)
	].FillHeight(1.f);

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .VAlign(VAlign_Fill)
		  .FillHeight(1.f)
		[
			VerticalBox.ToSharedRef()
		]

		+ SVerticalBox::Slot()
		  .VAlign(VAlign_Bottom)
		  .AutoHeight()
		  .Padding(-2.f, 0.f)
		[
			MakeStatusBar()
		]
	];
}

void SFavroDashboard::RegisterToolbarEntries(FSlimHorizontalToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.AddWidget(SNew(SFavroCollectionCard, Model.ToSharedRef()));
	ToolbarBuilder.AddSeparator();

	FF2UECommands::AddMenuEntry(ToolbarBuilder,
	                            FF2UECommands::Get().ToggleLeftPane,
	                            FUIAction(FExecuteAction::CreateSP(this,
	                                                               &SFavroDashboard::TogglePane, LEFT_PANE)));

	FF2UECommands::AddMenuEntry(ToolbarBuilder,
	                            FF2UECommands::Get().ToggleRightPane,
	                            FUIAction(FExecuteAction::CreateSP(this,
	                                                               &SFavroDashboard::TogglePane, RIGHT_PANE)));
}

TSharedRef<SDockTab> SFavroDashboard::HandleSpawnTab(const FSpawnTabArgs& Args, FName TabId)
{
	const bool bLeftPane = TabId == BacklogTabId;
	TSharedRef<SDockTab> DrawTab = SAssignNew(bLeftPane ? BacklogTab : BoardTab, SDockTab)
		.ShouldAutosize(false)
		.TabRole(PanelTab);

	const TSharedPtr<SWidget> TabWidget = SNew(SFavroBoardsList, Model.ToSharedRef()).bIsBacklog(bLeftPane);

	DrawTab->SetContent(TabWidget->AsShared());

	return DrawTab;
}


void SFavroDashboard::RegisterTabs()
{
	TabManager = FGlobalTabmanager::Get()->NewTabManager(DockTab.ToSharedRef());
	{
		TSharedRef<FWorkspaceItem> MenuGroup = TabManager->AddLocalWorkspaceMenuCategory(
			LOCTEXT("FavroLinkGroupName", "Favro Dashboard"));

		TabManager->RegisterTabSpawner(BacklogTabId,
		                               FOnSpawnTab::CreateRaw(this, &SFavroDashboard::HandleSpawnTab,
		                                                      BacklogTabId))
		          .SetDisplayName(LOCTEXT("BacklogTabTitle", "Left Pane"))
		          .SetGroup(MenuGroup)
		          .SetIcon(FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.LeftPane"))
		          .SetCanSidebarTab(false);

		TabManager->RegisterTabSpawner(BoardTabId,
		                               FOnSpawnTab::CreateRaw(this, &SFavroDashboard::HandleSpawnTab,
		                                                      BoardTabId))
		          .SetDisplayName(LOCTEXT("BoardTabTitle", "Right Pane"))
		          .SetGroup(MenuGroup)
		          .SetIcon(FSlateIcon(FF2UEStyle::GetStyleSetName(), "Favro.RightPane"))
		          .SetCanSidebarTab(false);
	}
}

TSharedRef<FTabManager::FLayout> SFavroDashboard::MakeLayout()
{
	return FTabManager::NewLayout("FavroLinkDashboardLayout_v0.1")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.5f)
				->Split(
					FTabManager::NewStack()
					->AddTab(BacklogTabId, ETabState::OpenedTab)
				)
			)
			->Split(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.5f)
				->Split(
					FTabManager::NewStack()
					->AddTab(BoardTabId, ETabState::OpenedTab)
				)
			)
		);
}

TSharedRef<SWidget> SFavroDashboard::MakeStatusBar()
{
	const FTableColumnHeaderStyle* HeaderStyle = &FAppStyle::Get().GetWidgetStyle<FTableColumnHeaderStyle>(
		"TableView.Header.Column");

	return SNew(SBorder)
		.BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SBox)[
					SNew(SBorder)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
					.BorderImage(&HeaderStyle->NormalBrush)
					.Padding(FMargin(10, 7, 8, 8))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						  .VAlign(VAlign_Center)
						  .HAlign(HAlign_Fill)
						  .FillWidth(.6f)
						  .Padding(8.f, 0.f)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Last Updated"))
							.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
						]

						+ SHorizontalBox::Slot()
						  .VAlign(VAlign_Center)
						  .AutoWidth()
						[
							SNew(STextBlock)
							.Text_Lambda([&]()
							                {
								                return FText::FromString(
									                Model->SelectedCollection.Dashboard.LastDashboardTimestamp.
									                       ToFormattedString(
										                       TEXT("%e %b %Y %H:%M%")));
							                })
							.TextStyle(FAppStyle::Get(), "SmallText")
						]
					]
				]
			]
		];
}

void SFavroDashboard::TogglePane(const bool bLeftPane)
{
	const TSharedRef<SDockTab> Tab = bLeftPane ? BacklogTab.ToSharedRef() : BoardTab.ToSharedRef();

	if (Tab->IsForeground())
	{
		Tab->RequestCloseTab();
	}
	else
	{
		Tab->GetParentDockTabStack()->OpenTab(Tab);
	}
}

EVisibility SFavroDashboard::IsPaneDrawerVisible(bool bLeftPane) const
{
	const bool bIsTabOpen = TabManager->FindExistingLiveTab(bLeftPane ? BacklogTabId : BoardTabId).IsValid();

	return bIsTabOpen ? EVisibility::Collapsed : EVisibility::Visible;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LEFT_PANE
#undef RIGHT_PANE

#undef LOCTEXT_NAMESPACE
