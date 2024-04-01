// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FavroTypes.h"
#include "WebImageCache.h"
#include "Widgets/SCompoundWidget.h"

class FFavroLinkModel;
/**
 * 
 */
class F2UE_API SFavroProfileCard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFavroProfileCard)
		{
		}

	SLATE_END_ARGS()

	virtual ~SFavroProfileCard() override;

	void RefreshImage(const FFavroOrganization& FavroOrganization);
	void Construct(const FArguments& InArgs, const TSharedRef<FFavroLinkModel>& InModel);

	FString LastUrl;

	TSharedPtr<FFavroLinkModel> Model;
	TSharedPtr<SImage> Image;
	FWebImageCache ImageCache;
};
