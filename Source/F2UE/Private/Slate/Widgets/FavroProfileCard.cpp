// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/FavroProfileCard.h"

#include "F2UEStyle.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SFavroProfileCard::~SFavroProfileCard()
{
	Model->OnOrganizationChanged.RemoveAll(this);
}

void SFavroProfileCard::RefreshImage(const FFavroOrganization& FavroOrganization)
{
	if (FavroOrganization.ThumbnailUrl.IsEmpty())
	{
		Image->SetImage(FF2UEStyle::GetBrush("Favro.Organization"));
		LastUrl = FavroOrganization.ThumbnailUrl;
		return;
	}

	if (LastUrl == FavroOrganization.ThumbnailUrl)
	{
		return;
	}

	LastUrl = FavroOrganization.ThumbnailUrl;
	Image->SetImage(ImageCache.Download(FavroOrganization.ThumbnailUrl)->Attr());
}

void SFavroProfileCard::Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel)
{
	Model = InModel;

	Model->OnOrganizationChanged.AddSP(this, &SFavroProfileCard::RefreshImage);
	LastUrl = Model->SelectedOrganization.ThumbnailUrl;

	ImageCache.SetDefaultStandInBrush(FF2UEStyle::GetBrush("Favro.Organization"));

	Image = SNew(SImage)
					.Image(ImageCache.Download(LastUrl)->Attr())
					.DesiredSizeOverride(FVector2D(24.f));

	ChildSlot
		[
			SNew(SBorder)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		.BorderImage(FF2UEStyle::Get().GetBrush("Favro.RoundedBackground"))
		.Padding(8, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .AutoWidth()
				  .VAlign(VAlign_Center)
				  .HAlign(HAlign_Center)
				[
					Image.ToSharedRef()
				]
				+ SHorizontalBox::Slot()
				  .AutoWidth()
				  .Padding(8.f, 0.f, 0.f, 0.f)
				  .VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(STextBlock)
                    	.Text(Model.ToSharedRef(), &FFavroLinkModel::GetOrganizationName)
                    	.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
					]

					+ SVerticalBox::Slot()[
						SNew(STextBlock)
                    	.Text(Model.ToSharedRef(), &FFavroLinkModel::GetUserName)
                    	.TextStyle(FAppStyle::Get(), "SmallText")
					]
				]

			]
		]
		.
		HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(8, 0);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
