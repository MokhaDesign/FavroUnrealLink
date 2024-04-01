#include "FavroLinkModel.h"

#include "F2UESettings.h"
#include "FavroMessenger.h"
#include "Slate/Public/Widgets/Input/SEditableTextBox.h"

FFavroLinkModel::FFavroLinkModel()
	: Messenger(MakeShareable(new FFavroMessenger()))
{
	Settings = GetMutableDefault<UF2UESettings>();

	if (!Settings->GetSelectedOrganization())
	{
		RequestOrganizations();
		return;
	}

	if (Settings->GetSelectedOrganization().Id.IsEmpty())
	{
		RequestOrganizations();
		return;
	}

	SelectedOrganization = Settings->GetSelectedOrganization();
	SelectedCollection = SelectedOrganization.Collections.IsEmpty()
		                     ? FFavroCollection()
		                     : SelectedOrganization.Collections.Last();
	SelectedUser = Settings->GetSelectedUser();

	Messenger->OnOrganizationsRequestCompleted.BindRaw(this, &FFavroLinkModel::OnOrganizationsRequestCompleted);
	Messenger->OnUserRequestCompleted.BindRaw(this, &FFavroLinkModel::OnUserRequestCompleted);
	Messenger->OnCollectionsRequestCompleted.BindRaw(this, &FFavroLinkModel::OnCollectionsRequestCompleted);
	Messenger->OnDashboardRequestCompleted.BindRaw(this, &FFavroLinkModel::OnDashboardRequestCompleted);

	if (IsCacheOutdated(EFavroEntities::Organizations))
	{
		RequestOrganizations();
	}
}

FReply FFavroLinkModel::HandleAuthenticate()
{
	RequestOrganizations(true);
	return FReply::Handled();
}

FReply FFavroLinkModel::HandleForceRefresh(const bool bCollections)
{
	if (bCollections)
	{
		RequestCollections(true);
	}
	else
	{
		RequestOrganizations(true);
	}

	return FReply::Handled();
}

void FFavroLinkModel::RequestOrganizations(const bool bForceRequest)
{
	if (IsCacheOutdated(EFavroEntities::Organizations) || bForceRequest)
	{
		bProcessingRequest = true;
		Messenger->MakeOrganizationsRequest();
	}
	else
	{
		OnOrganizationsRequestCompleted(true, Settings->Organizations);
	}
}

void FFavroLinkModel::RequestUsers()
{
	bProcessingRequest = true;
	Messenger->MakeUsersRequest();
}

void FFavroLinkModel::RequestCollections(const bool bForceRequest)
{
	if (IsCacheOutdated(EFavroEntities::Collection) || bForceRequest)
	{
		bProcessingRequest = true;
		Messenger->MakeCollectionsRequest();
	}
	else
	{
		OnCollectionsRequestCompleted(true, SelectedOrganization.Collections);
	}
}

void FFavroLinkModel::ChangeSelectedCollection(const FFavroCollection& Collection)
{
	if (Collection == nullptr)
	{
		return;
	}

	for (FFavroCollection& FavroCollection : SelectedOrganization.Collections)
	{
		if (FavroCollection.Id != Collection.Id)
		{
			continue;
		}

		SelectedCollection = FavroCollection;

		if (IsCacheOutdated(EFavroEntities::Dashboard))
		{
			bProcessingRequest = true;
			Messenger->MakeDashboardRequest(SelectedCollection.Id);
		}
		else
		{
			OnCollectionChanged.Broadcast(SelectedCollection);
		}
	}
}

void FFavroLinkModel::OnOrganizationsRequestCompleted(bool bWasSuccessful, TArray<FFavroOrganization> Organizations)
{
	if (bWasSuccessful)
	{
		if (SelectedOrganization != Settings->GetSelectedOrganization())
		{
			SelectedOrganization = Settings->GetSelectedOrganization();
			OnOrganizationChanged.Broadcast(SelectedOrganization);
		}

		Messenger->MakeUsersRequest();
	}
	bProcessingRequest = false;
}

void FFavroLinkModel::OnUserRequestCompleted(bool bWasSuccessful, FFavroUser User)
{
	if (bWasSuccessful)
	{
		SelectedUser = User;
		RequestCollections();
	}
	bProcessingRequest = false;
}

void FFavroLinkModel::OnCollectionsRequestCompleted(bool bWasSuccessful, TArray<FFavroCollection> Collections)
{
	if (bWasSuccessful)
	{
		SelectedOrganization = Settings->GetSelectedOrganization();
		OnOrganizationChanged.Broadcast(SelectedOrganization);

		if (SelectedUser.Name.IsEmpty())
		{
			SelectedUser = Settings->GetSelectedUser();
		}
	}
	bProcessingRequest = false;
}

void FFavroLinkModel::OnDashboardRequestCompleted(bool bWasSuccessful, FFavroDashboard Dashboard)
{
	if (bWasSuccessful)
	{
		SelectedOrganization = Settings->GetSelectedOrganization();
		OnOrganizationChanged.Broadcast(SelectedOrganization);

		for (FFavroCollection& FavroCollection : SelectedOrganization.Collections)
		{
			if (FavroCollection.Id == SelectedCollection.Id)
			{
				SelectedCollection = FavroCollection;
			}
		}

		OnCollectionChanged.Broadcast(SelectedCollection);
	}

	bProcessingRequest = false;
}


void FFavroLinkModel::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
                                       FProperty* PropertyThatChanged)
{
	FNotifyHook::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);

	if (!Settings || !PropertyThatChanged)
	{
		return;
	}

	if (PropertyThatChanged->GetName() == TEXT("OrganizationName"))
	{
		SelectedOrganization = Settings->GetSelectedOrganization();

		if (IsCacheOutdated(EFavroEntities::Collection))
		{
			Messenger->MakeCollectionsRequest();
		}
		else
		{
			OnOrganizationChanged.Broadcast(SelectedOrganization);
		}
	}
}

FText FFavroLinkModel::GetOrganizationName() const
{
	FString Name;
	if (!SelectedOrganization)
	{
		Name = "Invalid Organization";
	}
	else
	{
		Name = SelectedOrganization.Name.IsEmpty() ? "Invalid Organization" : SelectedOrganization.Name;
	}

	return FText::FromString(Name);
}

FText FFavroLinkModel::GetUserName() const
{
	FString Name;
	if (!SelectedUser)
	{
		Name = "Invalid User";
	}
	else
	{
		Name = SelectedUser.Name.IsEmpty() ? "Invalid User" : SelectedUser.Name;
	}

	return FText::FromString(Name);
}

FText FFavroLinkModel::GetCollectionName() const
{
	FString Name;
	if (!SelectedCollection)
	{
		Name = "Invalid Collection";
	}
	else
	{
		Name = SelectedCollection.Name.IsEmpty() ? "Invalid Collection" : SelectedCollection.Name;
	}

	return FText::FromString(Name);
}

bool FFavroLinkModel::IsCacheOutdated(const EFavroEntities& Entity) const
{
	const FDateTime CurrentTime = FDateTime::Now();
	FDateTime LastTime = FDateTime::Now();
	int Minutes = 0;

	switch (Entity)
	{
	case EFavroEntities::Organizations:
		LastTime = Settings->GetSelectedOrganization().LastCollectionsTimestamp;
		Minutes = Settings->OrganizationsInvalidateInterval;
		break;
	case EFavroEntities::Collection:
		LastTime = Settings->LastOrganizationsTimestamp;
		Minutes = Settings->CollectionsInvalidateInterval;
		break;
	case EFavroEntities::Dashboard:
		LastTime = SelectedCollection.Dashboard.LastDashboardTimestamp;
		Minutes = Settings->DashboardInvalidateInterval;
	default: ;
	}

	const FTimespan Difference = CurrentTime - LastTime;
	const FTimespan Threshold = FTimespan::FromMinutes(Minutes);

	return Difference > Threshold;
}
