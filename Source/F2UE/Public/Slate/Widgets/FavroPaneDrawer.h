// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/SlateTextBlockLayout.h"

class SFavroDashboard;
/**
 * This is mostly dupe of SRCPanelDrawer,
 * that is itself a modified version of STabSidebar.
 * The use here is minimal and simplified, since we know we need only 2.
 */
enum class EFavroPanelState
{
	Opened,
	Collapsed,
};

enum class EFavroPaneType
{
	Backlog,
	// Left Pane
	Board,
	// Right Pane
};

struct FFavroPaneDrawerArgs
{
	FFavroPaneDrawerArgs(EFavroPaneType InPanel)
		: bDrawnByDefault(false)
		  , DrawerVisibility(EVisibility::Visible)
		  , PanelID(InPanel)
		  , PanelState(EFavroPanelState::Collapsed)
	{
	}

	FFavroPaneDrawerArgs()
		: bDrawnByDefault(false)
		  , DrawerVisibility(EVisibility::Visible)
		  , PanelID(EFavroPaneType::Backlog)
		  , PanelState(EFavroPanelState::Collapsed)
	{
	}

	bool operator==(const FFavroPaneDrawerArgs& OtherPanel)
	{
		return PanelID == OtherPanel.GetPanelID();
	}

	bool operator==(const FFavroPaneDrawerArgs& OtherPanel) const
	{
		return PanelID == OtherPanel.GetPanelID();
	}

	EFavroPaneType GetPanelID() const
	{
		return PanelID;
	}

	bool IsActive() const
	{
		return PanelState == EFavroPanelState::Opened;
	}

	void SetState(EFavroPanelState NewState)
	{
		if (PanelState != NewState)
		{
			PanelState = NewState;
		}
	}

	bool bDrawnByDefault = false;
	EVisibility DrawerVisibility = EVisibility::Visible;

private:
	EFavroPaneType PanelID = EFavroPaneType::Backlog;
	EFavroPanelState PanelState = EFavroPanelState::Collapsed;
};

DECLARE_DELEGATE_OneParam(FOnFavroPaneDrawerButtonPressed, EFavroPaneType /* Panel */)

class F2UE_API SFavroPaneDrawer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroPaneDrawer)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FFavroPaneDrawerArgs>& InPanel,
	               const TSharedRef<SFavroDashboard>& InDashboard);

	void RegisterPanel(TSharedRef<FFavroPaneDrawerArgs> InPanel);
	void TogglePanel(EFavroPaneType FavroPaneType);

private:
	TSharedPtr<SVerticalBox> PanelBox;
	TSharedPtr<FFavroPaneDrawerArgs> Panel;
	TSharedPtr<SFavroDashboard> Dashboard;
};

/**
 * Vertical text block for use in the tab drawer button.
 * Text is aligned to the top of the widget if it fits without clipping;
 * otherwise it is ellipsized and fills the widget height.
 */
class SFavroPaneDrawerTextBlock : public SLeafWidget
{
public:
	enum class ERotation
	{
		Clockwise,
		CounterClockwise,
	};

	SLATE_BEGIN_ARGS(SFavroPaneDrawerTextBlock)
			: _Text()
			  , _TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
			  , _Rotation(ERotation::Clockwise)
			  , _OverflowPolicy()
		{
		}

		SLATE_ATTRIBUTE(FText, Text)

		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)

		SLATE_ATTRIBUTE(ERotation, Rotation)

		SLATE_ARGUMENT(TOptional<ETextOverflowPolicy>, OverflowPolicy)

	SLATE_END_ARGS()

	virtual ~SFavroPaneDrawerTextBlock() override
	{
	}

	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	void SetText(TAttribute<FText> InText)
	{
		Text = InText;
	}

	void SetRotation(TAttribute<ERotation> InRotation)
	{
		Rotation = InRotation;
	}

private:
	TAttribute<FText> Text;
	FTextBlockStyle TextStyle;
	TAttribute<ERotation> Rotation;
	TUniquePtr<FSlateTextBlockLayout> TextLayoutCache;
};

class SFavroPaneDrawerButton : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SFavroPaneDrawerButton)
		{
		}

		SLATE_EVENT(FOnFavroPaneDrawerButtonPressed, OnDrawerButtonPressed)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, TSharedRef<FFavroPaneDrawerArgs> ForPanel);
	void UpdateAppearance();

	virtual FSlateColor GetForegroundColor() const override
	{
		if (IsHovered())
		{
			return DockTabStyle->HoveredForegroundColor;
		}

		return FSlateColor::UseStyle();
	}

private:
	EVisibility GetActivePanelIndicatorVisibility() const
	{
		return Panel->IsActive() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
	}

	EVisibility GetPanelStateVisibility() const
	{
		return (IsHovered() || Panel->IsActive()) ? EVisibility::Visible : EVisibility::Hidden;
	}

	const FSlateBrush* GetPanelStateImage() const
	{
		if (Panel->GetPanelID() == EFavroPaneType::Board)
		{
			return FAppStyle::Get().GetBrush("Icons.ChevronLeft");
		}
		return FAppStyle::Get().GetBrush("Icons.ChevronRight");
	}

	TSharedPtr<FFavroPaneDrawerArgs> Panel;
	TSharedPtr<SFavroPaneDrawerTextBlock> Label;
	TSharedPtr<SButton> MainButton;
	FOnFavroPaneDrawerButtonPressed OnDrawerButtonPressed;
	const FDockTabStyle* DockTabStyle = nullptr;
};
