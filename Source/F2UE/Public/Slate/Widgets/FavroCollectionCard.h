// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroCollectionCard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroCollectionCard)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel);

	TSharedPtr<FFavroLinkModel> Model;
};
