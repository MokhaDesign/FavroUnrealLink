// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Tabs/FavroBoardsList.h"

#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"
#include "Slate/Widgets/FavroBoardsListView.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroBoardsList::Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel)
{
	Model = InModel;
	bIsBacklog = InArgs._bIsBacklog;

	Model->OnCollectionChanged.AddSP(this, &SFavroBoardsList::RefreshTree);

	const TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	VerticalBox->AddSlot()[
		SAssignNew(ListView, SFavroBoardsListView, Model)
		.ListItemsSource(&ListItems)
		.OnGenerateRow(this, &SFavroBoardsList::HandleGenerateRow)
		.SelectionMode(ESelectionMode::None)
	];

	RefreshTree(Model->SelectedCollection);

	if (!ListItems.IsEmpty())
	{
		ListView->SetSelection(ListItems[0]);
	}

	ChildSlot
	[
		VerticalBox.ToSharedRef()
	];
}


TSharedRef<ITableRow> SFavroBoardsList::HandleGenerateRow(TSharedPtr<FFavroBoard, ESPMode::ThreadSafe> FavroBoard,
                                                          const TSharedRef<STableViewBase, ESPMode::ThreadSafe>&
                                                          TableViewBase)
{
	return SNew(SFavroBoardsViewRow, ListView.ToSharedRef(), Model).Item(FavroBoard);
}

void SFavroBoardsList::RefreshTree(const FFavroCollection& SelectedCollection)
{
	TArray<FFavroBoard> Dashboard = bIsBacklog
		                                ? SelectedCollection.Dashboard.Backlog
		                                : SelectedCollection.Dashboard.Board;

	if (!ListItems.IsEmpty())
	{
		if (!Dashboard.IsEmpty())
		{
			if (ListItems[0]->Id == Dashboard[0].Id && ListItems.Num() == Dashboard.Num())
			{
				return;
			}
		}
	}

	ListItems.Empty();

	for (const FFavroBoard& Board : Dashboard)
	{
		ListItems.Add(MakeShared<FFavroBoard>(Board));
	}

	if (ListView.IsValid())
	{
		ListView->RebuildList();
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
