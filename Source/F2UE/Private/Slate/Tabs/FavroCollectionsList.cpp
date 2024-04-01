// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Tabs/FavroCollectionsList.h"

#include "F2UEStyle.h"
#include "FavroTypes.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"
#include "Slate/Widgets/FavroCollectionsTreeView.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SFavroCollectionsList::~SFavroCollectionsList()
{
	Model->OnOrganizationChanged.RemoveAll(this);
}

void SFavroCollectionsList::Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel)
{
	Model = InModel;
	Model->OnOrganizationChanged.AddSP(this, &SFavroCollectionsList::RefreshTree);

	const TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	VerticalBox->AddSlot()[MakeHeaderRow()].AutoHeight();

	VerticalBox->AddSlot()[
		SAssignNew(TreeView, SFavroCollectionsTreeView, Model)
		.ItemHeight(20.f)
		.TreeItemsSource(&TreeItems)
		.OnGenerateRow(this, &SFavroCollectionsList::HandleGenerateRow)
		.OnGetChildren(this, &SFavroCollectionsList::HandleGetChildren)
		.OnSelectionChanged(this, &SFavroCollectionsList::HandleSelectionChanged)
		.SelectionMode(ESelectionMode::Single)
	];

	RefreshTree(Model->SelectedOrganization);

	if (!TreeItems.IsEmpty())
	{
		TreeView->SetSelection(TreeItems[0]);
	}

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Fill)
			  .FillHeight(1.f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
				[
					VerticalBox.ToSharedRef()
				]
			]

			+ SVerticalBox::Slot()
			  .VAlign(VAlign_Bottom)
			  .AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
				[
					MakeStatusBar()
				]
			]
		]
	];
}

void SFavroCollectionsList::RefreshTree(const FFavroOrganization& SelectedOrganization)
{
	TreeItems.Empty();

	for (int i = SelectedOrganization.Collections.Num() - 1; i >= 0; --i)
	{
		const FFavroCollection& Collection = SelectedOrganization.Collections[i];
		TreeItems.Add(MakeShared<FFavroCollection>(Collection));
	}


	if (TreeView.IsValid())
	{
		TreeView->RebuildList();
	}

	if (!TreeItems.IsEmpty())
	{
		TreeView->SetSelection(TreeItems[0]);
	}
}

TSharedRef<SWidget> SFavroCollectionsList::MakeHeaderRow()
{
	const FTableColumnHeaderStyle* HeaderStyle = &FAppStyle::Get().GetWidgetStyle<FTableColumnHeaderStyle>(
		"TableView.Header.Column");

	return SNew(SBox)
	[
		SNew(SBorder)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		.BorderImage(&HeaderStyle->NormalBrush)
		.Padding(FMargin(10, 7, 8, 8))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Fill)
			  .FillWidth(1.f)
			[
				SNew(STextBlock)
                .Text(Model.ToSharedRef(), &FFavroLinkModel::GetOrganizationName)
                .TextStyle(FAppStyle::Get(), "ButtonText")
				.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
			]
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Right)
			  .AutoWidth()
			  .Padding(4.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.ToolTipText(FText::FromString("Force a Collection refresh"))
				.OnClicked(FOnClicked::CreateRaw(Model.Get(), &FFavroLinkModel::HandleForceRefresh, true))
				.ContentPadding(FMargin(1, 0))
				.IsEnabled_Lambda([&]() { return IsRefreshEnabled(); })
				[
					SNew(SImage)
					.Image_Lambda([&]()
					            {
						            return IsRefreshEnabled()
							                   ? FF2UEStyle::GetBrush("Favro.RefreshEnabled")
							                   : FF2UEStyle::GetBrush("Favro.RefreshDisabled");
					            })
					.ColorAndOpacity(FSlateColor::UseForeground())
				]
			]
		]
	];
}

TSharedRef<SWidget> SFavroCollectionsList::MakeStatusBar()
{
	const FTableColumnHeaderStyle* HeaderStyle = &FAppStyle::Get().GetWidgetStyle<FTableColumnHeaderStyle>(
		"TableView.Header.Column");


	return SNew(SBox)[
		SNew(SBorder)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		.BorderImage(&HeaderStyle->NormalBrush)
		.Padding(FMargin(10, 7, 8, 8))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Fill)
			  .FillWidth(.6f)
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
						                Model->SelectedOrganization.LastCollectionsTimestamp.ToFormattedString(
							                TEXT("%e %b %Y %H:%M%")));
				                })
				.TextStyle(FAppStyle::Get(), "SmallText")
			]]
	];
}

TSharedRef<ITableRow> SFavroCollectionsList::HandleGenerateRow(
	TSharedPtr<FFavroCollection, ESPMode::ThreadSafe> FavroCollection,
	const TSharedRef<STableViewBase, ESPMode::ThreadSafe>& TableViewBase)
{
	return SNew(SFavroCollectionsViewRow, TreeView.ToSharedRef()).Item(FavroCollection);
}

void SFavroCollectionsList::HandleGetChildren(TSharedPtr<FFavroCollection, ESPMode::ThreadSafe> FavroCollection,
                                              TArray<TSharedPtr<FFavroCollection, ESPMode::ThreadSafe>>& OutChildren)
{
	// No Children
}

void SFavroCollectionsList::HandleSelectionChanged(TSharedPtr<FFavroCollection, ESPMode::ThreadSafe> FavroCollection,
                                                   ESelectInfo::Type Arg)
{
	if (FavroCollection != nullptr)
	{
		if (Model->SelectedCollection.Id != FavroCollection->Id)
		{
			Model->ChangeSelectedCollection(*FavroCollection.Get());
		}
	}
}

bool SFavroCollectionsList::IsRefreshEnabled() const
{
	return !Model.Get()->IsProcessingRequest();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
