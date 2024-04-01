// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroLinkSettings : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroLinkSettings)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel);
	TSharedRef<SWidget> MakeCategoryHeader(const FString& Label) const;
	TSharedRef<SWidget> MakePropertyRow(const TSharedPtr<ISinglePropertyView>& WidgetIn,
	                                    const bool bPasswordToggle = false) const;

	TSharedRef<SWidget> MakeButton(const FString& ButtonLabel, FOnClicked OnClickedDelegate) const;

	void SetTextFieldVisibility(ECheckBoxState CheckBoxState,
	                            TSharedPtr<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle,
	                            TSharedPtr<ISinglePropertyView> WidgetIn) const;
	void SetEditableTextPasswordAttribute(const TSharedPtr<SWidget>& Widget, bool bIsPassword) const;
	ECheckBoxState IsTextFieldVisible(TSharedPtr<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle) const;

	TSharedPtr<FFavroLinkModel> Model;
};
