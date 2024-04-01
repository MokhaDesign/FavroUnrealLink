// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SFavroCardsTreeView;
struct FFavroCard;
class FFavroLinkModel;
struct FFavroBoard;

using FFavroBoardPtr = TSharedPtr<FFavroBoard>;

/**
* 
*/
class F2UE_API SFavroBoardsListView : public SListView<FFavroBoardPtr>
{
public:
	TSharedPtr<FFavroLinkModel> Model;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedPtr<FFavroLinkModel>& ModelIn);
};

class F2UE_API SFavroBoardsViewRow : public STableRow<FFavroBoardPtr>
{
	SLATE_BEGIN_ARGS(SFavroBoardsViewRow)
			: _IsEditable(true)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FFavroBoard>, Item)
		SLATE_ARGUMENT(bool, IsEditable)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView,
	               const TSharedPtr<FFavroLinkModel>& ModelIn);

	virtual TSharedRef<SWidget> GenerateWidget();
	TSharedRef<SWidget> MakeHeaderRow();
	TSharedRef<SWidget> MakeTaskView();

	void RefreshTree();
	TSharedRef<ITableRow> HandleGenerateRow(TSharedPtr<FFavroCard, ESPMode::ThreadSafe> FavroCard,
	                                        const TSharedRef<STableViewBase, ESPMode::ThreadSafe>& TableViewBase);
	void HandleGetChildren(TSharedPtr<FFavroCard, ESPMode::ThreadSafe> FavroCard,
	                       TArray<TSharedPtr<FFavroCard, ESPMode::ThreadSafe>>& OutChildren);

private:
	const FSlateBrush* GetHeaderBrush() const;
	EVisibility GetTaskViewVisibility() const;
	FSlateColor GetIconColor() const;

public:
	TSharedPtr<FFavroLinkModel> Model;
	TSharedPtr<FFavroBoard> Item;

private:
	TSharedPtr<SCheckBox> ExpanderArrow;
	TSharedPtr<SBorder> Header;
	FString LastColor;

	TArray<TSharedPtr<FFavroCard>> TreeItems;
	TSharedPtr<SFavroCardsTreeView> TreeView;
};
