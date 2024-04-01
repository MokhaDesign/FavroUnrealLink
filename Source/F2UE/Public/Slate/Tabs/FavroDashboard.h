// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SFavroPaneDrawer;
class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroDashboard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroDashboard)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs,
	               const TSharedRef<SDockTab>& InDockTab,
	               const TSharedRef<FFavroLinkModel>& InModel,
	               const TSharedRef<FUICommandList>& InCommandList);

	void RegisterToolbarEntries(FSlimHorizontalToolBarBuilder& ToolbarBuilder);
	TSharedRef<SDockTab> HandleSpawnTab(const FSpawnTabArgs& Args, FName TabId);

	void RegisterTabs();
	TSharedRef<FTabManager::FLayout> MakeLayout();

	TSharedRef<SWidget> MakeStatusBar();

	void TogglePane(bool bLeftPane);
	EVisibility IsPaneDrawerVisible(bool bLeftPane) const;

	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<FTabManager> TabManager;
	TSharedPtr<SDockTab> DockTab;

	TSharedPtr<FFavroLinkModel> Model;
	TSharedPtr<SDockTab> BacklogTab;
	TSharedPtr<SDockTab> BoardTab;
};
