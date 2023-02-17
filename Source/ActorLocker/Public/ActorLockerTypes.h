// Copyright 2023 Gradess Games. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ActorLockerTypes.generated.h"

#define OLDER_THAN_UE_5_1 (ENGINE_MAJOR_VERSION < 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 1))

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

	friend FArchive& operator<<(FArchive& Archive, FLockerTreeItem& Item)
	{
		Archive << Item.bLocked;
		return Archive;
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


UENUM()
enum class EActorLockerInteractionType : uint8
{
	None,
	Outliner,
	Ignored
};
