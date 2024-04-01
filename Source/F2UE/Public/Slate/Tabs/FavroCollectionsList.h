// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FavroTypes.h"
#include "Widgets/SCompoundWidget.h"

class SFavroCollectionsTreeView;
struct FFavroCollection;
class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroCollectionsList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroCollectionsList)
		{
		}

	SLATE_END_ARGS()

	virtual ~SFavroCollectionsList() override;

	void Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel);

	void RefreshTree(const FFavroOrganization& SelectedOrganization);

	TSharedRef<SWidget> MakeHeaderRow();
	TSharedRef<SWidget> MakeStatusBar();
	TSharedRef<ITableRow> HandleGenerateRow(TSharedPtr<FFavroCollection, ESPMode::ThreadSafe> FavroCollection,
	                                        const TSharedRef<STableViewBase, ESPMode::ThreadSafe>& TableViewBase);
	void HandleGetChildren(TSharedPtr<FFavroCollection, ESPMode::ThreadSafe> FavroCollection,
	                       TArray<TSharedPtr<FFavroCollection, ESPMode::ThreadSafe>>& OutChildren);
	void HandleSelectionChanged(TSharedPtr<FFavroCollection, ESPMode::ThreadSafe> FavroCollection,
	                            ESelectInfo::Type Arg);

	bool IsRefreshEnabled() const;
	TSharedPtr<FFavroLinkModel> Model;
	TArray<TSharedPtr<FFavroCollection>> TreeItems;

	TSharedPtr<SFavroCollectionsTreeView> TreeView;
};
