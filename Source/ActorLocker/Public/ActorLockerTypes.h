﻿// Copyright Gradess Games. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ActorLockerTypes.generated.h"

struct ISceneOutlinerTreeItem;
DECLARE_LOG_CATEGORY_EXTERN(LogActorLockerManager, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogLockWidget, Log, All);


USTRUCT(BlueprintType)
struct FLockerTreeItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Locker Tree Item")
	bool bLocked = false;

	TWeakPtr<ISceneOutlinerTreeItem> NativeItem = nullptr;

public:
	FLockerTreeItem() {}
	
	FLockerTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InItem, bool bInLocked = false)
		: bLocked(bInLocked)
		, NativeItem(InItem)
	{
	}
	
	uint32 GetId() const;
	static uint32 GetId(const TWeakPtr<ISceneOutlinerTreeItem>& InItem);
	
	FORCEINLINE bool IsValid() const
	{
		return NativeItem.IsValid();
	}
	
	operator TSharedPtr<ISceneOutlinerTreeItem>() const
	{
		return NativeItem.Pin();
	}

	operator TWeakPtr<ISceneOutlinerTreeItem>() const
	{
		return NativeItem;
	}

	FLockerTreeItem& operator=(const TWeakPtr<ISceneOutlinerTreeItem>& InItem)
	{
		NativeItem = InItem;
		return *this;
	}
};