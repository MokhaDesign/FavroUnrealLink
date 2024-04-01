#include "FavroMessenger.h"

#include "F2UESettings.h"
#include "F2UEStyle.h"
#include "HttpModule.h"
#include "FavroTypes.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Base64.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void FFavroMessenger::PrepareRequest(const FString& Route, const bool bWithOrgId,
                                     TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request,
                                     const FFavroRequestArgs& Args)
{
	FString ParsedArgs;

	if (!Args.IsEmpty())
	{
		ParsedArgs.Append("?");
		for (auto& [Arg, Value] : Args)
		{
			ParsedArgs.Append(FString::Printf(TEXT("&%s=%s"), *Arg, *Value));
		}
	}

	Request->SetURL(ApiUrl + Route + ParsedArgs);

	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader(TEXT("Content-Type"), "application/json");

	// Basic Auth
	const FString UserCredentials = FString::Printf(TEXT("%s:%s"), *Settings->Username, *Settings->ApiToken);
	const FString AuthorizationHeader = TEXT("Basic ") + FBase64::Encode(UserCredentials);
	Request->SetHeader(TEXT("Authorization"), AuthorizationHeader);

	if (bWithOrgId && Settings->GetSelectedOrganization())
	{
		const FString OrganizationId = Settings->GetSelectedOrganization().Id;
		Request->SetHeader(TEXT("organizationId"), OrganizationId);
	}
}

void FFavroMessenger::MakeOrganizationsRequest()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FFavroMessenger::CompleteOrganizationsRequest);

	PrepareRequest("organizations", false, Request);

	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}


void FFavroMessenger::MakeUsersRequest()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FFavroMessenger::CompleteUserRequest);

	PrepareRequest("users", true, Request);

	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}

void FFavroMessenger::MakeCollectionsRequest()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FFavroMessenger::CompleteCollectionsRequest);

	FFavroRequestArgs Args;
	Args.Add("archived", "false");
	PrepareRequest("collections", true, Request, Args);

	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}

void FFavroMessenger::MakeDashboardRequest(const FString& CollectionId)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FFavroMessenger::CompleteDashboardRequest, CollectionId);

	FFavroRequestArgs Args;
	Args.Add("collectionId", CollectionId);
	Args.Add("archived", "false");
	PrepareRequest("widgets", true, Request, Args);

	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}

void FFavroMessenger::MakeBoardsRequest(const FString& CollectionId)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindRaw(this, &FFavroMessenger::CompleteBoardsRequest, CollectionId);

	FFavroRequestArgs Args;
	Args.Add("collectionId", CollectionId);
	Args.Add("archived", "false");
	PrepareRequest("cards", true, Request, Args);

	Request->SetVerb(TEXT("GET"));
	Request->ProcessRequest();
}

void FFavroMessenger::CompleteOrganizationsRequest(FHttpRequestPtr Request,
                                                   FHttpResponsePtr Response,
                                                   bool bWasSuccessful)
{
	if (!(bWasSuccessful && Response.IsValid()))
	{
		OnOrganizationsRequestCompleted.ExecuteIfBound(false, TArray<FFavroOrganization>());
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Response->GetContentAsString());

	if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
	{
		OnOrganizationsRequestCompleted.ExecuteIfBound(false, TArray<FFavroOrganization>());
		return;
	}

	TArray<FFavroOrganization> Organizations;

	const TArray<TSharedPtr<FJsonValue>>* Entities;
	if (!JsonObject->TryGetArrayField(TEXT("entities"), Entities))
	{
		OnOrganizationsRequestCompleted.ExecuteIfBound(false, TArray<FFavroOrganization>());
		return;
	}

	for (const TSharedPtr<FJsonValue>& Entity : *Entities)
	{
		TSharedPtr<FJsonObject> EntityObj = Entity->AsObject();
		FFavroOrganization Org;

		Org.Id = EntityObj->GetStringField(TEXT("organizationId"));
		Org.Name = EntityObj->GetStringField(TEXT("name"));
		if (EntityObj->HasField(TEXT("thumbnail")))
		{
			Org.ThumbnailUrl = EntityObj->GetStringField(TEXT("thumbnail"));
		}

		const TArray<TSharedPtr<FJsonValue>>* Users;
		if (!EntityObj->TryGetArrayField(TEXT("sharedToUsers"), Users))
		{
			continue;
		}

		for (const TSharedPtr<FJsonValue>& UserValue : *Users)
		{
			TSharedPtr<FJsonObject> UserObj = UserValue->AsObject();
			FFavroUser User;
			User.Id = UserObj->GetStringField(TEXT("userId"));

			Org.Users.Add(User);
		}

		Organizations.Add(Org);
	}

	Settings->Organizations.Empty();

	Settings->Organizations = Organizations;
	Settings->LastOrganizationsTimestamp = FDateTime::Now();
	Settings->SaveConfig();

	if (!Settings->GetOrganizationNames().Contains(Settings->OrganizationName))
	{
		Settings->OrganizationName = Settings->GetOrganizationNames()[0];
		Settings->SaveConfig();
	}

	OnOrganizationsRequestCompleted.ExecuteIfBound(true, Organizations);
}

void FFavroMessenger::CompleteUserRequest(FHttpRequestPtr Request,
                                          FHttpResponsePtr Response,
                                          bool bWasSuccessful)
{
	if (!(bWasSuccessful && Response.IsValid()))
	{
		OnUserRequestCompleted.ExecuteIfBound(false, FFavroUser());
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Response->GetContentAsString());

	if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
	{
		OnUserRequestCompleted.ExecuteIfBound(false, FFavroUser());
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* UsersArray;
	if (!JsonObject->TryGetArrayField(TEXT("entities"), UsersArray))
	{
		OnUserRequestCompleted.ExecuteIfBound(false, FFavroUser());
		return;
	}

	const FString SelectedEmail = Settings->Username;

	for (const TSharedPtr<FJsonValue>& UserValue : *UsersArray)
	{
		const TSharedPtr<FJsonObject> UserObj = UserValue->AsObject();
		const FString Email = UserObj->GetStringField(TEXT("email"));
		const FString UserId = UserObj->GetStringField(TEXT("userId"));
		const FString Name = UserObj->GetStringField(TEXT("name"));

		for (FFavroUser& FavroUser : Settings->GetSelectedOrganization().Users)
		{
			if (FavroUser != *UserId)
			{
				continue;
			}

			FavroUser.Id = UserId;
			FavroUser.Name = Name;
			FavroUser.Email = Email;

			if (Email.Equals(SelectedEmail))
			{
				OnUserRequestCompleted.ExecuteIfBound(true, FavroUser);
			}
		}
	}
}

void FFavroMessenger::CompleteCollectionsRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
                                                 TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
                                                 bool bWasSuccessful)
{
	if (!(bWasSuccessful && Response.IsValid()))
	{
		OnCollectionsRequestCompleted.ExecuteIfBound(false, TArray<FFavroCollection>());
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Response->GetContentAsString());

	if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
	{
		OnCollectionsRequestCompleted.ExecuteIfBound(false, TArray<FFavroCollection>());
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* UsersArray;
	if (!JsonObject->TryGetArrayField(TEXT("entities"), UsersArray))
	{
		OnCollectionsRequestCompleted.ExecuteIfBound(false, TArray<FFavroCollection>());
		return;
	}

	TArray<FFavroCollection> ParsedCollections;

	for (const TSharedPtr<FJsonValue>& Value : *UsersArray)
	{
		const TSharedPtr<FJsonObject> CollectionObj = Value->AsObject();
		if (!CollectionObj.IsValid())
		{
			continue;
		}
		FFavroCollection Collection;
		Collection.Name = CollectionObj->GetStringField(TEXT("name"));
		Collection.Id = CollectionObj->GetStringField(TEXT("collectionId"));
		Collection.SharingLevel = ConvertToSharingLevel(CollectionObj->GetStringField(TEXT("publicSharing")));

		const TArray<TSharedPtr<FJsonValue>>* SharedUsersArray;
		if (!CollectionObj->TryGetArrayField(TEXT("sharedToUsers"), SharedUsersArray))
		{
			continue;
		}

		for (const TSharedPtr<FJsonValue>& UserValue : *SharedUsersArray)
		{
			const TSharedPtr<FJsonObject> UserObj = UserValue->AsObject();
			if (UserObj.IsValid())
			{
				FString UserId;
				if (UserObj->HasField(TEXT("userId")))
				{
					UserId = UserObj->GetStringField(TEXT("userId"));
				}
				else
				{
					// If it is not an user, it is a org-wide specifier
					UserId = UserObj->GetStringField(TEXT("type"));
				}

				EFavroCollectionUserRole UserRole = ConvertToUserRole(UserObj->GetStringField(TEXT("role")));

				Collection.Users.Add(UserId, UserRole);
			}
		}

		Collection.LastCollectionTimestamp = FDateTime::Now();

		ParsedCollections.Add(Collection);
	}

	FFavroOrganization& SelectedOrganization = Settings->GetSelectedOrganization();
	SelectedOrganization.Collections.Reset();
	SelectedOrganization.Collections = ParsedCollections;
	SelectedOrganization.LastCollectionsTimestamp = FDateTime::Now();

	MakeDashboardRequest(ParsedCollections.Last().Id);

	Settings->SaveConfig();

	OnCollectionsRequestCompleted.ExecuteIfBound(true, ParsedCollections);
}

void FFavroMessenger::CompleteDashboardRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
                                               TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
                                               bool bWasSuccessful, FString CollectionId)
{
	if (!(bWasSuccessful && Response.IsValid()))
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Response->GetContentAsString());

	if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* BoardsArray;
	if (!JsonObject->TryGetArrayField(TEXT("entities"), BoardsArray))
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	TArray<FFavroBoard> ParsedBacklog;
	TArray<FFavroBoard> ParsedBoards;

	for (const TSharedPtr<FJsonValue>& BoardValue : *BoardsArray)
	{
		FFavroBoard ParsedBoard = FFavroBoard();
		const TSharedPtr<FJsonObject> BoardObj = BoardValue->AsObject();
		const bool bBacklog = BoardObj->GetStringField(TEXT("type")) == "backlog";
		ParsedBoard.Id = BoardObj->GetStringField(TEXT("widgetCommonId"));
		ParsedBoard.Name = BoardObj->GetStringField(TEXT("name"));
		BoardObj->TryGetStringArrayField("collectionIds", ParsedBoard.Collections);
		ParsedBoard.SharingLevel = ConvertToSharingLevel(
			BoardObj->GetStringField(TEXT("ownerRole")));
		ParsedBoard.EditLevel =
			ConvertToSharingLevel(BoardObj->GetStringField(TEXT("editRole")));
		ParsedBoard.Color = BoardObj->GetStringField(TEXT("color"));

		if (bBacklog)
		{
			ParsedBacklog.Add(ParsedBoard);
		}
		else
		{
			ParsedBoards.Add(ParsedBoard);
		}
	}

	FFavroCollection& Collection = Settings->GetCollection(CollectionId);
	if (!Collection)
	{
		return;
	}

	Collection.LastCollectionTimestamp = FDateTime::Now();
	auto& [Backlog, Board, Timestamp] = Collection.Dashboard;

	if (!Backlog.IsEmpty())
	{
		Backlog.Empty();
	}
	Backlog.Append(ParsedBacklog);

	if (!Board.IsEmpty())
	{
		Board.Empty();
	}
	Board.Append(ParsedBoards);

	Timestamp = FDateTime::Now();
	Settings->SaveConfig();

	MakeBoardsRequest(CollectionId);
}

void FFavroMessenger::CompleteBoardsRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request,
                                            TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response,
                                            bool bWasSuccessful, FString CollectionId)
{
	if (!(bWasSuccessful && Response.IsValid()))
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Response->GetContentAsString());

	if (!(FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()))
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* CardsArray;
	if (!JsonObject->TryGetArrayField(TEXT("entities"), CardsArray))
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	FFavroCollection& Collection = Settings->GetCollection(CollectionId);
	if (!Collection)
	{
		OnDashboardRequestCompleted.ExecuteIfBound(false, FFavroDashboard());
		return;
	}

	for (const TSharedPtr<FJsonValue>& CardValue : *CardsArray)
	{
		FFavroCard ParsedCard = FFavroCard();
		const TSharedPtr<FJsonObject> CardObj = CardValue->AsObject();

		ParsedCard.Id = CardObj->GetStringField(TEXT("cardId"));
		ParsedCard.InstanceId = CardObj->GetStringField(TEXT("cardCommonId"));
		ParsedCard.Name = CardObj->GetStringField(TEXT("name"));
		ParsedCard.BoardId = CardObj->HasField(TEXT("widgetCommonId"))
			                     ? CardObj->GetStringField(TEXT("widgetCommonId"))
			                     : CardObj->GetStringField(TEXT("todoListUserId"));

		if (CardObj->HasField(TEXT("laneId")))
		{
			CardObj->TryGetStringField(TEXT("laneId"), ParsedCard.ParentId);
		}
		else if (CardObj->HasField(TEXT("columnId")))
		{
			CardObj->TryGetStringField(TEXT("columnId"), ParsedCard.ParentId);
		}
		else if (CardObj->HasField(TEXT("parentCardId")))
		{
			CardObj->TryGetStringField(TEXT("parentCardId"), ParsedCard.ParentId);
		}

		ParsedCard.Position = CardObj->HasField(TEXT("listPosition"))
			                      ? CardObj->GetNumberField(TEXT("listPosition"))
			                      : CardObj->GetNumberField(TEXT("sheetPosition"));
		CardObj->TryGetStringField(TEXT("detailedDescription"), ParsedCard.Description);
		CardObj->TryGetStringField(TEXT("sequentialId"), ParsedCard.UrlId);

		if (CardObj->HasField(TEXT("numComments")))
		{
			ParsedCard.NumComments = CardObj->GetIntegerField(TEXT("numComments"));
		}
		if (CardObj->HasField(TEXT("tasksTotal")))
		{
			ParsedCard.NumTasks = CardObj->GetIntegerField(TEXT("tasksTotal"));
		}
		if (CardObj->HasField(TEXT("tasksCompleted")))
		{
			ParsedCard.TasksCompleted = CardObj->GetIntegerField(TEXT("tasksCompleted"));
		}


		const TArray<TSharedPtr<FJsonValue>>* Tags;
		if (CardObj->TryGetArrayField(TEXT("tags"), Tags))
		{
			for (const TSharedPtr<FJsonValue>& TagValue : *Tags)
			{
#if 0
				// @TODO(Marian) MODEL - This will be done so in its own request
				const TSharedPtr<FJsonObject> TagObj = TagValue->AsObject();
				FString Tag;
				FString Color;
				TagObj->TryGetStringField(TEXT("name"), Tag);
				TagObj->TryGetStringField(TEXT("color"), Color);

				if (!Tag.IsEmpty())
				{
					ParsedCard.Tags.Add(Tag, Color);
				}
#endif
				const FString Tag = TagValue->AsString();
				if (!Tag.IsEmpty())
				{
					ParsedCard.Tags.Add(Tag);
				}
			}
		}

		const TArray<TSharedPtr<FJsonValue>>* Assignments;
		if (JsonObject->TryGetArrayField(TEXT("assignments"), Assignments))
		{
			for (const TSharedPtr<FJsonValue>& AssignmentValue : *Assignments)
			{
				const TSharedPtr<FJsonObject> AssignmentObj = AssignmentValue->AsObject();
				FString UserId;
				bool bCompleted;
				AssignmentObj->TryGetStringField(TEXT("userId"), UserId);
				AssignmentObj->TryGetBoolField(TEXT("completed"), bCompleted);

				if (!UserId.IsEmpty())
				{
					ParsedCard.Assignments.Add(UserId, bCompleted);
				}
			}
		}

		for (FFavroBoard& Board : Collection.Dashboard.Backlog)
		{
			if (Board.Id == ParsedCard.BoardId)
			{
				Board.Cards.Add(ParsedCard);
			}
		}

		for (FFavroBoard& Board : Collection.Dashboard.Board)
		{
			if (Board.Id == ParsedCard.BoardId)
			{
				Board.Cards.Add(ParsedCard);
			}
		}
	}

	Collection.Dashboard.LastDashboardTimestamp = FDateTime::Now();
	Settings->SaveConfig();

	OnDashboardRequestCompleted.ExecuteIfBound(true, Collection.Dashboard);
}

EFavroCollectionSharingLevel FFavroMessenger::ConvertToSharingLevel(const FString& String)
{
	if (String == "users" || String == "owners")
	{
		return EFavroCollectionSharingLevel::Users;
	}
	if (String == "organization" || String == "fullMembers")
	{
		return EFavroCollectionSharingLevel::Organization;
	}

	return EFavroCollectionSharingLevel::Public;
}

EFavroCollectionUserRole FFavroMessenger::ConvertToUserRole(const FString& String)
{
	if (String == "view")
	{
		return EFavroCollectionUserRole::View;
	}
	if (String == "edit")
	{
		return EFavroCollectionUserRole::Edit;
	}
	if (String == "admin")
	{
		return EFavroCollectionUserRole::Admin;
	}
	return EFavroCollectionUserRole::Guest;
}
