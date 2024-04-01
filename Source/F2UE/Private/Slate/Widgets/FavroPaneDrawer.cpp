// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/Widgets/FavroPaneDrawer.h"

#include "F2UEStyle.h"
#include "SlateOptMacros.h"
#include "Framework/Text/PlainTextLayoutMarshaller.h"
#include "Framework/Text/SlateTextLayoutFactory.h"
#include "Slate/Tabs/FavroDashboard.h"
#include "Widgets/Text/SlateTextBlockLayout.h"

const FVector2D MaxPanelSize(144.f, 25.f);

#define LOCTEXT_NAMESPACE "FavroPaneDrawer"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

/**
 * Vertical text block for use in the tab drawer button.
 * Text is aligned to the top of the widget if it fits without clipping;
 * otherwise it is ellipsized and fills the widget height.
 */
void SFavroPaneDrawerTextBlock::Construct(const FArguments& InArgs)
{
	Text = InArgs._Text;
	TextStyle = *InArgs._TextStyle;
	Rotation = InArgs._Rotation;
	TextLayoutCache = MakeUnique<FSlateTextBlockLayout>(
		this, FTextBlockStyle::GetDefault(), TOptional<ETextShapingMethod>(), TOptional<ETextFlowDirection>(),
		FCreateSlateTextLayout(), FPlainTextLayoutMarshaller::Create(), nullptr);
	TextLayoutCache->SetTextOverflowPolicy(InArgs._OverflowPolicy.IsSet()
		                                       ? InArgs._OverflowPolicy
		                                       : TextStyle.OverflowPolicy);
}

int32 SFavroPaneDrawerTextBlock::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                         const FSlateRect& MyCullingRect,
                                         FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                         const FWidgetStyle& InWidgetStyle,
                                         bool bParentEnabled) const
{
	// We're going to figure out the bounds of the corresponding horizontal text, and then rotate it into a vertical orientation.
	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	const FVector2D DesiredHorizontalTextSize = TextLayoutCache->GetDesiredSize();
	const FVector2D ActualHorizontalTextSize(FMath::Min(DesiredHorizontalTextSize.X, LocalSize.Y),
	                                         FMath::Min(DesiredHorizontalTextSize.Y, LocalSize.X));

	// Now determine the center of the vertical text by rotating the dimensions of the horizontal text.
	// The center should align it to the top of the widget.
	const FVector2D VerticalTextSize(ActualHorizontalTextSize.Y, ActualHorizontalTextSize.X);
	const FVector2D VerticalTextCenter = VerticalTextSize / 2.f;

	// Now determine where the horizontal text should be positioned so that it is centered on the vertical text:
	//      +-+
	//      |v|
	//      |e|
	// [ horizontal ]
	//      |r|
	//      |t|
	//      +-+
	const FVector2D HorizontalTextPosition = VerticalTextCenter - ActualHorizontalTextSize / 2.f;

	// Define the text's geometry using the horizontal bounds, then rotate it 90/-90 degrees into place to become vertical.
	const FSlateRenderTransform RotationTransform(
		FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(Rotation.Get() == ERotation::Clockwise ? 90 : -90))));
	const FGeometry TextGeometry = AllottedGeometry.MakeChild(ActualHorizontalTextSize,
	                                                          FSlateLayoutTransform(HorizontalTextPosition),
	                                                          RotationTransform, FVector2D(0.5f, 0.5f));

	return TextLayoutCache->OnPaint(Args, TextGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                                ShouldBeEnabled(bParentEnabled));
}

FVector2D SFavroPaneDrawerTextBlock::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	// The text's desired size reflects the horizontal/untransformed text.
	// Switch the dimensions for vertical text.
	const FVector2D DesiredHorizontalTextSize = TextLayoutCache->ComputeDesiredSize(
		FSlateTextBlockLayout::FWidgetDesiredSizeArgs(
			Text.Get(),
			FText(),
			0.f,
			false,
			ETextWrappingPolicy::DefaultWrapping,
			ETextTransformPolicy::None,
			FMargin(),
			1.f,
			ETextJustify::Left),
		LayoutScaleMultiplier, TextStyle);
	return FVector2D(DesiredHorizontalTextSize.Y, DesiredHorizontalTextSize.X);
}


void SFavroPaneDrawerButton::Construct(const FArguments& InArgs, TSharedRef<FFavroPaneDrawerArgs> ForPanel)
{
	const FVector2D Size = MaxPanelSize;

	DockTabStyle = &FAppStyle::Get().GetWidgetStyle<FDockTabStyle>("Docking.Tab");
	OnDrawerButtonPressed = InArgs._OnDrawerButtonPressed;

	Panel = ForPanel;

	const FString Text = Panel->GetPanelID() == EFavroPaneType::Backlog ? "Left Pane" : "Right Pane";
	const FString IconName = Panel->GetPanelID() == EFavroPaneType::Backlog ? "Favro.LeftPane" : "Favro.RightPane";
	const FSlateBrush* Icon = FF2UEStyle::GetBrush(*IconName);

	TSharedRef<SBox> Box = SNew(SBox)
		.WidthOverride(Size.Y) // Swap desired dimensions for a vertical tab
		.HeightOverride(Size.X)
		.Clipping(EWidgetClipping::ClipToBounds);

	const FMargin ContentPadding = FMargin(0.f, DockTabStyle->TabPadding.Top, 0.f,
	                                       DockTabStyle->TabPadding.Bottom);

	auto OnPressed = [this]()
	{
		OnDrawerButtonPressed.ExecuteIfBound(
			Panel->GetPanelID());
	};

	Box->SetContent(SAssignNew(MainButton, SButton)
		.ContentPadding(ContentPadding)
		.OnPressed_Lambda(OnPressed)
		.ForegroundColor(FSlateColor::UseForeground())
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Center)
			  .Padding(0.f, 5.f, 0.f, 5.f)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(Icon)
				.DesiredSizeOverride(FVector2D(16, 16))
			]
			+ SVerticalBox::Slot()
			  .Padding(0.f, 5.f, 0.f, 5.f)
			  .FillHeight(1.f)
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SAssignNew(Label, SFavroPaneDrawerTextBlock)
				.TextStyle(&DockTabStyle->TabTextStyle)
				.Text(FText::FromString(Text))
				.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				.Clipping(EWidgetClipping::ClipToBounds)
			]
			+ SVerticalBox::Slot()
			  .AutoHeight()
			  .HAlign(HAlign_Center)
			  .Padding(0.f, 5.f, 0.f, 4.f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("NoBorder"))
				.Visibility(this, &SFavroPaneDrawerButton::GetPanelStateVisibility)
				.Padding(2.f)
				.HAlign(HAlign_Center)
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(this, &SFavroPaneDrawerButton::GetPanelStateImage)
				]
			]
		]);

	ChildSlot
		.Padding(0.f)
		[
			Box
		];

	UpdateAppearance();
}

void SFavroPaneDrawerButton::UpdateAppearance()
{
	const SFavroPaneDrawerTextBlock::ERotation Rotation = Panel->GetPanelID() == EFavroPaneType::Backlog
		                                                      ? SFavroPaneDrawerTextBlock::ERotation::Clockwise
		                                                      : SFavroPaneDrawerTextBlock::ERotation::CounterClockwise;

	check(Label);
	Label->SetRotation(Rotation);
	MainButton->SetButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Docking.SidebarButton.Closed"));
}

void SFavroPaneDrawer::Construct(const FArguments& InArgs, const TSharedRef<FFavroPaneDrawerArgs>& InPanel,
                                 const TSharedRef<SFavroDashboard>& InDashboard)
{
	Dashboard = InDashboard;

	ChildSlot
		.Padding(0.f)
		[
			SNew(SBorder)
			.Padding(0.f)
			.BorderImage(FAppStyle::Get().GetBrush("Docking.Sidebar.Background"))
			[
				SAssignNew(PanelBox, SVerticalBox)
			]
		];

	RegisterPanel(InPanel);
}

void SFavroPaneDrawer::RegisterPanel(TSharedRef<FFavroPaneDrawerArgs> InPanel)
{
	TSharedRef<SFavroPaneDrawerButton> PanelButton = SNew(SFavroPaneDrawerButton, InPanel)
		.OnDrawerButtonPressed(this, &SFavroPaneDrawer::TogglePanel);

	PanelBox->AddSlot()
	        // Make the tabs evenly fill the sidebar until they reach the max size
	        .FillHeight(1.f)
	        .MaxHeight(MaxPanelSize.X)
	        .HAlign(HAlign_Left)
	        .Padding(0.f, 0.f, 0.f, 4.f)
	[
		PanelButton
	];

	Panel = InPanel;

	if (InPanel->bDrawnByDefault)
	{
		InPanel->SetState(EFavroPanelState::Opened);
	}

	if (InPanel->IsActive())
	{
		PanelButton->UpdateAppearance();
	}
}

void SFavroPaneDrawer::TogglePanel(EFavroPaneType FavroPaneType)
{
	Dashboard->TogglePane(FavroPaneType == EFavroPaneType::Backlog);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
