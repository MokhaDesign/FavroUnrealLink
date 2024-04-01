#pragma once

#include "FavroTypes.h"
#include "Delegates/Delegate.h"

class UF2UESettings;
class FFavroMessenger;

class FFavroLinkModel final : public FNotifyHook
{
public:
	FFavroLinkModel();

	// Authenticate
	FReply HandleAuthenticate();
	FReply HandleForceRefresh(const bool bCollections);

	// Messenger
	void RequestOrganizations(const bool bForceRequest = false);
	void RequestUsers();
	void RequestCollections(const bool bForceRequest = false);

	// Settings
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnOrganizationChanged, const FFavroOrganization&)
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCollectionChanged, const FFavroCollection&)
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent,
	                              FProperty* PropertyThatChanged) override;

	// Utils
	FText GetOrganizationName() const;
	FText GetUserName() const;
	FText GetCollectionName() const;

	bool IsCacheOutdated(const EFavroEntities& Entity) const;
	bool IsProcessingRequest() const { return bProcessingRequest; }

	void ChangeSelectedCollection(const FFavroCollection& Collection);

private:
	// Messenger
	void OnOrganizationsRequestCompleted(bool bWasSuccessful, TArray<FFavroOrganization> Organizations);
	void OnUserRequestCompleted(bool bWasSuccessful, FFavroUser User);
	void OnCollectionsRequestCompleted(bool bWasSuccessful, TArray<FFavroCollection> Collections);
	void OnDashboardRequestCompleted(bool bWasSuccessful, FFavroDashboard Dashboard);

public:
	// @TODO(Marian) MODEL - TSharedPtr<>?
	FFavroOrganization SelectedOrganization;
	FFavroUser SelectedUser;
	FFavroCollection SelectedCollection;

	FOnOrganizationChanged OnOrganizationChanged;
	FOnCollectionChanged OnCollectionChanged;

private:
	TSharedPtr<FFavroMessenger> Messenger;
	TObjectPtr<UF2UESettings> Settings;

	bool bProcessingRequest = false;
};
