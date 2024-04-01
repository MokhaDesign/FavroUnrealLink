// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FavroTypes.h"
#include "Engine/DeveloperSettings.h"
#include "F2UESettings.generated.h"

/**
 * 
 */

struct FFavroOrganization;
struct FFavroUser;

UCLASS(Config=FavroLink, meta=(DisplayName="Favro Link Settings"))
class F2UE_API UF2UESettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UF2UESettings();

	//~ Begin UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FName GetSectionName() const override;
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
	//~ End UDeveloperSettings interface

	/** Get Mutable CDO of UF2UESettings */
	static UF2UESettings& Get();

	UPROPERTY(Config, meta=(PasswordField=false))
	FString Username;

	UPROPERTY(Config, meta=(PasswordField=true))
	FString ApiToken;

	UPROPERTY(Config, DisplayName="Selected Organization", meta=(GetOptions="GetOrganizationNames"))
	FString OrganizationName;

	UPROPERTY(Config)
	TArray<FFavroOrganization> Organizations;

	UPROPERTY(Config)
	FDateTime LastOrganizationsTimestamp;

	UPROPERTY(Config, meta=(ClampMin="0", UIMin="0", Units="Minutes", ForceUnits="Minutes"))
	int OrganizationsInvalidateInterval = 30;

	UPROPERTY(Config, meta=(ClampMin="0", UIMin="0", Units="Minutes", ForceUnits="Minutes"))
	int CollectionsInvalidateInterval = 15;

	UPROPERTY(Config, meta=(ClampMin="0", UIMin="0", Units="Minutes", ForceUnits="Minutes"))
	int DashboardInvalidateInterval = 5;

	UFUNCTION()
	TArray<FString> GetOrganizationNames() const;

	UFUNCTION()
	FFavroOrganization& GetSelectedOrganization();

	UFUNCTION()
	FFavroCollection& GetCollection(const FString& CollectionId);

	UFUNCTION()
	FFavroUser GetSelectedUser();

	UFUNCTION()
	FString GetSelectedUserName();
};
