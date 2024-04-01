// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

struct FFavroCollection;
class FFavroLinkModel;

using FFavroCollectionPtr = TSharedPtr<FFavroCollection>;
/**
 * 
 */
class F2UE_API SFavroCollectionsTreeView : public STreeView<FFavroCollectionPtr>
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FFavroLinkModel>& ModelIn);

	TSharedPtr<FFavroLinkModel> Model;
};

class F2UE_API SFavroCollectionsViewRow : public STableRow<FFavroCollectionPtr>
{
	SLATE_BEGIN_ARGS(SFavroCollectionsViewRow)
			: _IsEditable(true)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FFavroCollection>, Item)
		SLATE_ARGUMENT(bool, IsEditable)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
	const FSlateBrush* GetCollectionIcon() const;
	virtual TSharedRef<SWidget> GenerateWidget();

	TSharedPtr<FFavroCollection> Item;

	const FSlateBrush* SharingUsersBrush;
	const FSlateBrush* SharingOrganizationBrush;
	const FSlateBrush* SharingPublicBrush;
};
