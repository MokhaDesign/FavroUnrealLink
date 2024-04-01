// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

struct FFavroCard;
class FFavroLinkModel;

using FFavroCardPtr = TSharedPtr<FFavroCard>;

/**
 * 
 */
class F2UE_API SFavroCardsTreeView : public STreeView<FFavroCardPtr>
{
public:
	void Construct(const FArguments& InArgs, const TSharedPtr<FFavroLinkModel>& ModelIn);

	TSharedPtr<FFavroLinkModel> Model;
};

class F2UE_API SFavroCardsViewRow : public STableRow<FFavroCardPtr>
{
	SLATE_BEGIN_ARGS(SFavroCardsViewRow)
			: _IsEditable(true)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FFavroCard>, Item)
		SLATE_ARGUMENT(bool, IsEditable)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
	virtual TSharedRef<SWidget> GenerateWidget();

	TSharedRef<SWidget> ConstructTitleRow();
	TSharedRef<SWidget> ConstructTagsRow();
	TSharedRef<SWidget> ConstructDescriptionRow();

private:
	const FSlateBrush* GetBorderBrush() const;

private:
	TSharedPtr<SBorder> Background;
	TSharedPtr<FFavroCard> Item;
};

class F2UE_API SFavroTagChip : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SFavroTagChip)
		{
		}

		SLATE_ARGUMENT(FString, Label)
		SLATE_ARGUMENT(FSlateColor, Color)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FString Label;
	FSlateColor Color;
};
