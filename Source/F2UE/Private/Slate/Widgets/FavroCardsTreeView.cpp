// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/FavroCardsTreeView.h"

#include "F2UEStyle.h"
#include "FavroTypes.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Text/SMultiLineEditableText.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SFavroCardsTreeView::Construct(const FArguments& InArgs, const TSharedPtr<FFavroLinkModel>& ModelIn)
{
	Model = ModelIn;

	STreeView::Construct(InArgs);
}

void SFavroCardsViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Item = InArgs._Item;

	STableRow<FFavroCardPtr>::Construct(STableRow<FFavroCardPtr>::FArguments(), InOwnerTableView);

	ChildSlot
	[
		SNew(SBox)
		.Padding(4.f)
		[
			GenerateWidget()
		]
	];
}

TSharedRef<SWidget> SFavroCardsViewRow::GenerateWidget()
{
	if (!Item.IsValid())
	{
		return SNullWidget::NullWidget;
	}

	return
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderImage(FF2UEStyle::Get().GetBrush("Favro.CardBackground"))
			.Padding(16.f, 8.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Fill)
				  .HAlign(HAlign_Fill)
				  .FillHeight(1.f)
				[
					ConstructTitleRow()
				]

				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Bottom)
				  .HAlign(HAlign_Fill)
				  .AutoHeight()
				  .Padding(0.f, Item->Tags.IsEmpty() ? 0.f : 4.f, 0.f, 0.f)
				[
					ConstructTagsRow()
				]

				+ SVerticalBox::Slot()
				  .VAlign(VAlign_Bottom)
				  .HAlign(HAlign_Fill)
				  .AutoHeight()
				  .Padding(0.f, Item->Description.IsEmpty() ? 0.f : 4.f, 0.f, 0.f)
				[
					ConstructDescriptionRow()
				]
			]
		]
		+ SOverlay::Slot()
		[
			SAssignNew(Background, SBorder)
			.BorderImage(this, &SFavroCardsViewRow::GetBorderBrush)
		];
}

TSharedRef<SWidget> SFavroCardsViewRow::ConstructTitleRow()
{
	return SNew(STextBlock).Text(FText::FromString(Item->Name));
}

TSharedRef<SWidget> SFavroCardsViewRow::ConstructTagsRow()
{
	if (Item->Tags.IsEmpty())
	{
		return SNullWidget::NullWidget;
	}

	const TSharedRef<SWrapBox> WrapBox =
		SNew(SWrapBox)
		.Orientation(Orient_Horizontal)
		.UseAllottedSize(true)
		.InnerSlotPadding(FVector2d(8.f, 4.f));
	const FSlateBrush* BorderBrush = new FSlateRoundedBoxBrush(FFavroColors::Blue, 4.f, FStyleColors::Secondary, 1.f);

	int Index = 0;

	for (FString TagId : Item->Tags)
	{
		Index++;
		FString DummyString = FString::Printf(TEXT("Tag %i"), Index);

		WrapBox->AddSlot()
		[
			SNew(SBorder)
			.BorderImage(BorderBrush)
			.BorderBackgroundColor(FLinearColor(.5f, .5f, .5f, 1.0f))
			.Padding(8.f, 2.f)
			[
				SNew(STextBlock)
				.TextStyle(FAppStyle::Get(), "SmallText")
				.Text(FText::FromString(*DummyString))
			]
		];
	}

#if 0
	// @TODO(Marian) SLATE - This is how we'll do once we have proper Tags type
	for (auto& [TagLabel, Color] : Item->Tags)
	{
		const FSlateColor TagColor = FF2UEStyle::ConvertToSlateColor(Color);
		WrapBox->AddSlot()
		[
			SNew(SFavroTagChip)
			.Label(TagLabel)
			.Color(TagColor)
		];
	}

#endif

	return WrapBox;
}

TSharedRef<SWidget> SFavroCardsViewRow::ConstructDescriptionRow()
{
	if (Item->Description.IsEmpty())
	{
		return SNullWidget::NullWidget;
	}

	TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

	const FString PreviewString = Item->Description.Replace(TEXT("\n"), TEXT(" "));

	HorizontalBox->AddSlot()
	             .Padding(0.f, 0.f, 4.f, 0.f)
	             .VAlign(VAlign_Center)
	             .HAlign(HAlign_Left)
	             .AutoWidth()
	[
		SNew(SImage)
				.Image(FF2UEStyle::GetBrush("Favro.CardDescription"))
				.ColorAndOpacity(FSlateColor::UseForeground())
	];

	HorizontalBox->AddSlot()
	             .VAlign(VAlign_Center)
	             .HAlign(HAlign_Fill)
	             .FillWidth(1.f)
	[
		SNew(STextBlock)
		.Text(FText::FromString(PreviewString))
		.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
	];

	return HorizontalBox;
}

const FSlateBrush* SFavroCardsViewRow::GetBorderBrush() const
{
	const FButtonStyle& ButtonStyle = FAppStyle::GetWidgetStyle<FButtonStyle>("PlacementBrowser.Asset");

	if (!Background.IsValid())
	{
		return &ButtonStyle.Normal;
	}

	return Background->IsHovered() ? &ButtonStyle.Hovered : &ButtonStyle.Normal;
}

void SFavroTagChip::Construct(const FArguments& InArgs)
{
	Label = InArgs._Label;
	Color = InArgs._Color;

	const FSlateBrush* BorderBrush = new FSlateRoundedBoxBrush(Color, 4.f, FFavroColors::Secondary, 1.f);

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(BorderBrush)
		.BorderBackgroundColor(FLinearColor(.5f, .5f, .5f, 1.0f))
		.Padding(8.f, 2.f)
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), "SmallText")
			.Text(FText::FromString(*Label))
		]
	];
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
