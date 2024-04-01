#pragma once
#include "FavroTypes.h"
#include "HttpFwd.h"
#include "Interfaces/IHttpRequest.h"

class UF2UESettings;
struct FFavroCollection;
struct FFavroOrganization;
struct FFavroUser;
class IHttpResponse;
class IHttpRequest;

class FFavroMessenger
{
public:
	FFavroMessenger() : Settings(GetMutableDefault<UF2UESettings>())
	{
	}

	DECLARE_DELEGATE_TwoParams(FOnOrganizationsRequestCompleted, bool /*bWasSuccessful*/,
	                           TArray<FFavroOrganization> /*Orgs*/);
	DECLARE_DELEGATE_TwoParams(FOnUserRequestCompleted, bool /*bWasSuccessful*/, FFavroUser /*User*/);
	DECLARE_DELEGATE_TwoParams(FOnCollectionsRequestCompleted, bool /*bWasSuccessful*/,
	                           TArray<FFavroCollection> /*Collections*/);
	DECLARE_DELEGATE_TwoParams(FOnDashboardRequestCompleted, bool /*bWasSuccessful*/,
	                           FFavroDashboard /*Dashboard*/);

	TSharedPtr<UF2UESettings> Settings;

	FOnOrganizationsRequestCompleted OnOrganizationsRequestCompleted;
	FOnUserRequestCompleted OnUserRequestCompleted;
	FOnCollectionsRequestCompleted OnCollectionsRequestCompleted;
	FOnDashboardRequestCompleted OnDashboardRequestCompleted;

	void PrepareRequest(const FString& Route, const bool bWithOrgId,
	                    TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request,
	                    const FFavroRequestArgs& Args = FFavroRequestArgs());

	void MakeOrganizationsRequest();
	void MakeUsersRequest();
	void MakeCollectionsRequest();
	void MakeDashboardRequest(const FString& CollectionId);
	void MakeBoardsRequest(const FString& CollectionId);

	void CompleteOrganizationsRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                                  TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	                                  bool bWasSuccessful);

	void CompleteUserRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                         TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	                         bool bWasSuccessful);

	void CompleteCollectionsRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                                TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	                                bool bWasSuccessful);

	void CompleteDashboardRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                              TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	                              bool bWasSuccessful, FString CollectionId);

	void CompleteBoardsRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
	                           TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
	                           bool bWasSuccessful, FString CollectionId);

	static EFavroCollectionSharingLevel ConvertToSharingLevel(const FString& String);
	static EFavroCollectionUserRole ConvertToUserRole(const FString& String);
	static FSlateColor ConvertToSlateColor(const FString& String);
};
