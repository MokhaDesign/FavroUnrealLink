// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/FavroCollectionsTreeView.h"

#include "F2UEStyle.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroCollectionsTreeView::Construct(const FArguments& InArgs, const TSharedPtr<FFavroLinkModel>& ModelIn)
{
	Model = ModelIn;

	STreeView::Construct(InArgs);
}

void SFavroCollectionsViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Item = InArgs._Item;

	SharingUsersBrush = FF2UEStyle::GetBrush("Favro.Private");
	SharingOrganizationBrush = FF2UEStyle::GetBrush("Favro.Collection");
	SharingPublicBrush = FF2UEStyle::GetBrush("Favro.Global");

	STableRow<FFavroCollectionPtr>::Construct(STableRow<FFavroCollectionPtr>::FArguments(), InOwnerTableView);

	ChildSlot
	[
		GenerateWidget()
	];
}

TSharedRef<SWidget> SFavroCollectionsViewRow::GenerateWidget()
{
	if (!Item.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	return SNew(SBorder)
	.BorderImage(FAppStyle::GetBrush("NoBorder"))
	.Padding(2.f, 6.f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		  .Padding(6.f, 0.f)
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		  .HAlign(HAlign_Center)
		[
			SNew(SImage)
			.Image(this, &SFavroCollectionsViewRow::GetCollectionIcon)
			.ColorAndOpacity(FSlateColor::UseForeground())
		]

		+ SHorizontalBox::Slot()
		  .Padding(2.f, 1.f, 2.f, 0.f)
		  .AutoWidth()
		  .VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(Item->Name))
			.Font(FAppStyle::GetFontStyle("ContentBrowser.SourceTreeItemFont"))
		]
	];
}

const FSlateBrush* SFavroCollectionsViewRow::GetCollectionIcon() const
{
	switch (Item->SharingLevel)
	{
	case EFavroCollectionSharingLevel::Users: return SharingUsersBrush;
	case EFavroCollectionSharingLevel::Organization: return SharingOrganizationBrush;
	default: return SharingPublicBrush;
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
