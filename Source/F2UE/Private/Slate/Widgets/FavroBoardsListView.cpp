// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/FavroBoardsListView.h"

#include "F2UEStyle.h"
#include "FavroTypes.h"
#include "SlateOptMacros.h"
#include "Slate/Widgets/FavroCardsTreeView.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroBoardsListView::Construct(const FArguments& InArgs, const TSharedPtr<FFavroLinkModel>& ModelIn)
{
	Model = ModelIn;

	SListView::Construct(InArgs);
}

void SFavroBoardsViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView,
                                    const TSharedPtr<FFavroLinkModel>& ModelIn)
{
	Item = InArgs._Item;
	Model = ModelIn;

	STableRow<FFavroBoardPtr>::Construct(STableRow<FFavroBoardPtr>::FArguments(), InOwnerTableView);

	ChildSlot
	[
		GenerateWidget()
	];
}

TSharedRef<SWidget> SFavroBoardsViewRow::GenerateWidget()
{
	if (!Item.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	const TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	VerticalBox->AddSlot()[
			MakeHeaderRow()
		]
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill);

	VerticalBox->AddSlot()[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			MakeTaskView()
		].Visibility(this, &SFavroBoardsViewRow::GetTaskViewVisibility)
	];

	RefreshTree();

	return SNew(SBorder)
	.BorderImage(FAppStyle::GetBrush("NoBorder"))
	.Padding(8.f, 6.f)
	[
		VerticalBox
	];
}

TSharedRef<SWidget> SFavroBoardsViewRow::MakeHeaderRow()
{
	return SNew(SBox)
	[
		SAssignNew(Header, SBorder)
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		.BorderImage(this, &SFavroBoardsViewRow::GetHeaderBrush)
		.Padding(FMargin(10, 6, 8, 6))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .Padding(4.f, 0.f)
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Left)
			  .AutoWidth()
			[
				SAssignNew(ExpanderArrow, SCheckBox)
				.Style(FF2UEStyle::Get(), "Favro.ExpanderStyle")
				.IsChecked(ECheckBoxState::Checked)
			]

			+ SHorizontalBox::Slot()
			  .Padding(0.f, 0.f, 4.f, 0.f)
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Left)
			  .AutoWidth()
			[
				SNew(SImage)
				.Image(FF2UEStyle::GetBrush("Favro.CardBoard"))
				.ColorAndOpacity(this, &SFavroBoardsViewRow::GetIconColor)
			]

			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Fill)
			  .FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Item->Name))
				.TextStyle(FAppStyle::Get(), "ButtonText")
				.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
			]
		]
	];
}

TSharedRef<SWidget> SFavroBoardsViewRow::MakeTaskView()
{
	return SAssignNew(TreeView, SFavroCardsTreeView, Model)
	.TreeItemsSource(&TreeItems)
	.OnGenerateRow(this, &SFavroBoardsViewRow::HandleGenerateRow)
	.OnGetChildren(this, &SFavroBoardsViewRow::HandleGetChildren)
	.SelectionMode(ESelectionMode::None);
}

void SFavroBoardsViewRow::RefreshTree()
{
	TreeItems.Empty();

	for (FFavroCard& Card : Item->Cards)
	{
		TreeItems.Add(MakeShared<FFavroCard>(Card));
	}

	if (!TreeItems.IsEmpty())
	{
		TreeItems.Sort(
			[](const TSharedPtr<FFavroCard>& A,
			   const TSharedPtr<FFavroCard>& B)
			{
				return A->Position < B->Position;
			});
	}

	if (TreeView.IsValid())
	{
		TreeView->RebuildList();
	}
}

TSharedRef<ITableRow> SFavroBoardsViewRow::HandleGenerateRow(TSharedPtr<FFavroCard, ESPMode::ThreadSafe> FavroCard,
                                                             const TSharedRef<STableViewBase, ESPMode::ThreadSafe>&
                                                             TableViewBase)
{
	return SNew(SFavroCardsViewRow, TreeView.ToSharedRef()).Item(FavroCard);
}

void SFavroBoardsViewRow::HandleGetChildren(TSharedPtr<FFavroCard, ESPMode::ThreadSafe> FavroCard,
                                            TArray<TSharedPtr<FFavroCard, ESPMode::ThreadSafe>>& OutChildren)
{
	// @TODO(Marian) SLATE - Come back when we have Hierarchical structure
}

const FSlateBrush* SFavroBoardsViewRow::GetHeaderBrush() const
{
	const FTableColumnHeaderStyle* HeaderStyle = &FAppStyle::Get().GetWidgetStyle<FTableColumnHeaderStyle>(
		"TableView.Header.Column");

	if (!Header.IsValid())
	{
		return &HeaderStyle->NormalBrush;
	}

	return Header->IsHovered() ? &HeaderStyle->HoveredBrush : &HeaderStyle->NormalBrush;
}

EVisibility SFavroBoardsViewRow::GetTaskViewVisibility() const
{
	if (!ExpanderArrow.IsValid())
	{
		return EVisibility::Visible;
	}

	return ExpanderArrow->IsChecked() ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor SFavroBoardsViewRow::GetIconColor() const
{
	return FF2UEStyle::ConvertToSlateColor(Item->Color);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
