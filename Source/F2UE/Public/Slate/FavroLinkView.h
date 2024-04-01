// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FavroTypes.h"
#include "Widgets/SCompoundWidget.h"

class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroLinkView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroLinkView)
		{
		}

	SLATE_END_ARGS()

	SFavroLinkView();

	/** Constructs this widget with InArgs */
	void Construct(
		const FArguments& InArgs,
		const TSharedRef<SDockTab>& MajorTab,
		const TSharedPtr<SWindow>& Window,
		const TSharedPtr<FFavroLinkModel> InModel);

	void RegisterToolbarEntries(FSlimHorizontalToolBarBuilder& ToolbarBuilder) const;
	void RegisterMenus(FMenuBarBuilder& MenuBarBuilder);
	void RegisterTabs(const TSharedRef<SDockTab>& MajorTab);
	TSharedRef<FTabManager::FLayout> MakeLayout() const;

	TSharedRef<SDockTab> HandleSpawnTab(const FSpawnTabArgs& Args, FName TabId);
	static void FillWindowMenu(FMenuBuilder& MenuBuilder, TSharedPtr<FTabManager, ESPMode::ThreadSafe> TabManager);
	static void FillActionsMenu(FMenuBuilder& MenuBuilder);

	static void HandleAddNewTask();
	static void HandleAddBugReport();

	void HandleCollectionChanged(const FFavroCollection& FavroOrganization);

	TSharedRef<FUICommandList> CommandList;
	TSharedPtr<FTabManager> TabManager;

	TSharedPtr<FFavroLinkModel> Model;
};
