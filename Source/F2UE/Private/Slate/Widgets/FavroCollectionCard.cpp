// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/FavroCollectionCard.h"

#include "F2UEStyle.h"
#include "SlateOptMacros.h"
#include "Models/FavroLinkModel.h"
#include "Widgets/Images/SLayeredImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroCollectionCard::Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel)
{
	Model = InModel;

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
					SNew(SLayeredImage)
					.Image(FF2UEStyle::GetBrush("Favro.Collection"))
					.ColorAndOpacity(FSlateColor::UseForeground())
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
                    	.Text(Model.ToSharedRef(), &FFavroLinkModel::GetCollectionName)
                    	.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
					]

					// + SVerticalBox::Slot()[
					// 	SNew(STextBlock)
					//                	.Text(Model.ToSharedRef(), &FFavroLinkModel::GetUserName)
					//                	.TextStyle(FAppStyle::Get(), "SmallText")
					// ]
				]

			]
		]
		.
		HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(8, 0);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
