// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FavroTypes.generated.h"

struct FFavroUser;

using FFavroRequestArgs = TMap<FString, FString>;
static const FString ApiUrl = TEXT("https://favro.com/api/v1/");

UENUM()
enum class EFavroEntities : uint8
{
	Organizations,
	Collection,
	Dashboard
};

UENUM()
enum class EFavroCollectionSharingLevel : uint8
{
	Users,
	Organization,
	Public
};

UENUM()
enum class EFavroCollectionUserRole : uint8
{
	Guest,
	View,
	Edit,
	Admin
};

// Every Board holds Cards
// @TODO(Marian) TYPES - Largely Incomplete
USTRUCT()
struct FFavroCard
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	// Shared Id for Instancing
	UPROPERTY()
	FString InstanceId;

	UPROPERTY()
	FString Name;

	// Either
	// - widgetCommonId if not in a To-do,
	// - todoListUserId otherwise
	UPROPERTY()
	FString BoardId;

	// Only valid for todoList cards
	// UPROPERTY()
	// bool bCompleted;

	// @TODO(Marian) SLATE - For TreeView implementation 
	// Either:
	// - Kanban Column Id
	// - Lane Id
	// - Parent Card Id in Hierarchical
	UPROPERTY()
	FString ParentId;

	// UPROPERTY()
	// bool bIsLane;

	// Either:
	// - listPosition if in To-do or Kanban
	// - sheetPosition if in a Sheet or Card List
	UPROPERTY()
	float Position = 0.f;

	// The body of the card. Sometimes in Markdown, sometimes not.
	UPROPERTY()
	FString Description;

	// @TODO(Marian) TYPES - TagIds to Tag Info
	// It looks like Favro didn't update their docu
	// Since the card request returns only an array of tag Ids
	// And you need to do a different request for the information...
	// Tag : Colour
	UPROPERTY()
	TArray<FString> Tags;

	// sequentialId - Id used only for URL creation.
	UPROPERTY()
	FString UrlId;

	UPROPERTY()
	FDateTime StartDate;

	UPROPERTY()
	FDateTime DueDate;

	// User : Completed Map
	UPROPERTY()
	TMap<FString, bool> Assignments;

	UPROPERTY()
	int NumComments = 0;

	UPROPERTY()
	int NumTasks = 0;

	UPROPERTY()
	int TasksCompleted = 0;

	// @TODO(Marian) TYPES - Incomplete or omitted: attachments, dependencies, customFields, timeOnBoard, timeOnColumns, favroAttachments
};

// Every Dashboard has 2 Boards
USTRUCT()
struct FFavroBoard
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	TArray<FString> Collections;

	UPROPERTY()
	TArray<FFavroCard> Cards;

	UPROPERTY()
	FString Color;

	UPROPERTY()
	EFavroCollectionSharingLevel SharingLevel = EFavroCollectionSharingLevel::Organization;

	UPROPERTY()
	EFavroCollectionSharingLevel EditLevel = EFavroCollectionSharingLevel::Organization;
};

// Every Collection has a Dashboard
USTRUCT()
struct FFavroDashboard
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FFavroBoard> Backlog; // Left Pane

	UPROPERTY()
	TArray<FFavroBoard> Board; // Right Pane

	UPROPERTY()
	FDateTime LastDashboardTimestamp;
};

USTRUCT()
struct FFavroCollection
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	EFavroCollectionSharingLevel SharingLevel = EFavroCollectionSharingLevel::Organization;

	UPROPERTY()
	TMap<FString, EFavroCollectionUserRole> Users;

	UPROPERTY()
	FFavroDashboard Dashboard;

	UPROPERTY()
	FDateTime LastCollectionTimestamp;

	bool operator==(const FString& Other) const
	{
		return Id == Other;
	}

	bool operator!=(const FString& Other) const
	{
		return Id != Other;
	}

	// @TODO(Marian) OPERATORS - All of those don't work - Look into other solutions
	explicit operator bool() const { return !Id.IsEmpty(); }
	bool operator==(nullptr_t) const { return Id.IsEmpty(); }
	bool operator!=(nullptr_t) const { return !Id.IsEmpty(); }
};

USTRUCT()
struct FFavroUser
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Email;

	bool operator==(const FString& Other) const
	{
		return Id == Other;
	}

	bool operator!=(const FString& Other) const
	{
		return Id != Other;
	}

	// @TODO(Marian) OPERATORS - All of those don't work - Look into other solutions
	explicit operator bool() const { return !Id.IsEmpty(); }
	bool operator==(nullptr_t) const { return Id.IsEmpty(); }
	bool operator!=(nullptr_t) const { return !Id.IsEmpty(); }
};

USTRUCT()
struct FFavroOrganization
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString ThumbnailUrl;

	UPROPERTY()
	TArray<FFavroUser> Users;

	UPROPERTY()
	TArray<FFavroCollection> Collections;

	UPROPERTY()
	FDateTime LastCollectionsTimestamp;

	bool operator==(const FString& Other) const
	{
		return Name == Other;
	}

	bool operator==(const FFavroOrganization& Rhs) const
	{
		return Rhs.Id == Id;
	}

	bool operator!=(const FFavroOrganization& Rhs) const
	{
		return Rhs.Id != Id;
	}

	// @TODO(Marian) OPERATORS - All of those don't work - Look into other solutions
	explicit operator bool() const { return !Id.IsEmpty(); }
	bool operator==(nullptr_t) const { return Id.IsEmpty(); }
	bool operator!=(nullptr_t) const { return !Id.IsEmpty(); }
};
