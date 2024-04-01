// Fill out your copyright notice in the Description page of Project Settings.


#include "F2UESettings.h"
#include "FavroTypes.h"

UF2UESettings::UF2UESettings()
{
	FCoreDelegates::OnPreExit.AddLambda([]
	{
		Get().SaveConfig();
	});
}

UF2UESettings& UF2UESettings::Get()
{
	UF2UESettings* MutableCDO = GetMutableDefault<UF2UESettings>();
	check(MutableCDO != nullptr)

	return *MutableCDO;
}

TArray<FString> UF2UESettings::GetOrganizationNames() const
{
	TArray<FString> OrganizationNames;

	if (Organizations.IsEmpty())
	{
		return OrganizationNames;
	}

	for (FFavroOrganization Organization : Organizations)
	{
		OrganizationNames.Add(Organization.Name);
	}

	return OrganizationNames;
}

FFavroOrganization& UF2UESettings::GetSelectedOrganization()
{
	if (OrganizationName.IsEmpty() && !GetOrganizationNames().IsEmpty())
	{
		OrganizationName = GetOrganizationNames()[0];
		SaveConfig();
	}

	if (Organizations.FindByKey(*OrganizationName) == nullptr)
	{
		if (Organizations.IsEmpty())
		{
			Organizations.AddDefaulted();
		}

		return Organizations[0];
	}

	return *Organizations.FindByKey(*OrganizationName);
}

FFavroCollection& UF2UESettings::GetCollection(const FString& CollectionId)
{
	FFavroOrganization& Organization = GetSelectedOrganization();

	if (Organization.Collections.FindByKey(CollectionId) == nullptr)
	{
		if (Organization.Collections.IsEmpty())
		{
			Organization.Collections.AddDefaulted();
		}

		return Organization.Collections[0];
	}

	return *Organization.Collections.FindByKey(CollectionId);
}

FFavroUser UF2UESettings::GetSelectedUser()
{
	for (const FFavroUser& User : GetSelectedOrganization().Users)
	{
		if (User.Email == Username)
		{
			return User;
		}
	}

	return FFavroUser();
}

FString UF2UESettings::GetSelectedUserName()
{
	for (auto [Id, Name, Email] : GetSelectedOrganization().Users)
	{
		if (!Email.Equals(Username))
		{
			return Name;
		}
	}

	return "Invalid User";
}

FName UF2UESettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FName UF2UESettings::GetSectionName() const
{
	return TEXT("Favro Link");
}

FText UF2UESettings::GetSectionText() const
{
	return NSLOCTEXT("F2UEPlugin", "F2UESettingsSection", "Favro Link Settings");
}

FText UF2UESettings::GetSectionDescription() const
{
	return NSLOCTEXT("F2UEPlugin", "F2UESettingsDescription", "Settings for the Favro Link plugin");
}
#endif
