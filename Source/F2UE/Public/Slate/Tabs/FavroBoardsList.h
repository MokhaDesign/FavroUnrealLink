// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FavroTypes.h"
#include "Slate/Widgets/FavroBoardsListView.h"
#include "Widgets/SCompoundWidget.h"

struct FFavroBoard;
class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroBoardsList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroBoardsList)
		{
		}

		SLATE_ARGUMENT(bool, bIsBacklog)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel);

	bool bIsBacklog;

	TSharedRef<ITableRow> HandleGenerateRow(TSharedPtr<FFavroBoard, ESPMode::ThreadSafe> FavroBoard,
	                                        const TSharedRef<STableViewBase, ESPMode::ThreadSafe>& TableViewBase);
	void RefreshTree(const FFavroCollection& SelectedCollection);

	TSharedPtr<FFavroLinkModel> Model;
	TArray<TSharedPtr<FFavroBoard>> ListItems;

	TSharedPtr<SListView<TSharedPtr<FFavroBoard>>> ListView;
};
