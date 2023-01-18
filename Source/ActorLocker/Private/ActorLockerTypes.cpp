// Copyright 2023 Gradess Games. All Rights Reserved.

#include "ActorLockerTypes.h"
#include "ActorTreeItem.h"
#include "FolderTreeItem.h"
#include "LevelTreeItem.h"
#include "WorldTreeItem.h"

DEFINE_LOG_CATEGORY(LogActorLockerManager)
DEFINE_LOG_CATEGORY(LogLockWidget)

uint32 FLockerTreeItem::GetId() const
{
	return GetId(NativeItem);
}

uint32 FLockerTreeItem::GetId(const TWeakPtr<ISceneOutlinerTreeItem>& InItem)
{
	if (!InItem.IsValid())
	{
		return 0;
	}
	
	const auto TreeItemPtr = InItem.Pin();
	if (const auto ActorTreeItem = TreeItemPtr->CastTo<FActorTreeItem>())
	{
		if (ActorTreeItem->Actor.IsValid())
		{
			return ActorTreeItem->Actor->GetUniqueID();
		}
	}
	else if (const auto LevelTreeItem = TreeItemPtr->CastTo<FLevelTreeItem>())
	{
		if (LevelTreeItem->Level.IsValid())
		{
			return LevelTreeItem->Level->GetUniqueID();
		}
	}
	else if (const auto WorldTreeItem = TreeItemPtr->CastTo<FWorldTreeItem>())
	{
		if (WorldTreeItem->World.IsValid())
		{
			return WorldTreeItem->World->GetUniqueID();
		}
	}
	else if (const auto FolderTreeItem = TreeItemPtr->CastTo<FFolderTreeItem>())
	{
		return FolderTreeItem->GetID().CalculateTypeHash();
	}
	
	return 0;
}
