// Copyright Gradess Games. All Rights Reserved.

#include "ActorLockerManager.h"
#include "ActorFolderTreeItem.h"
#include "ActorLocker.h"
#include "ActorLockerTypes.h"
#include "ActorTreeItem.h"
#include "LevelEditor.h"
#include "LevelTreeItem.h"
#include "Selection.h"
#include "SOutlinerTreeView.h"
#include "WorldTreeItem.h"

void UActorLockerManager::PostInitProperties()
{
	UObject::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	const auto Selection = GEditor->GetSelectedActors();
	Selection->SelectObjectEvent.AddUObject(this, &UActorLockerManager::OnActorSelected);
	Selection->SelectionChangedEvent.AddUObject(this, &UActorLockerManager::OnSelectionChanged);
	GEngine->OnLevelActorDeleted().AddUObject(this, &UActorLockerManager::OnActorDeleted);
	FSlateDebugging::InputEvent.AddUObject(this, &UActorLockerManager::OnInputEvent);
}

void UActorLockerManager::BeginDestroy()
{
	if (const auto Selection = GEditor->GetSelectedActors())
	{
		Selection->SelectObjectEvent.RemoveAll(this);
	}

	GEngine->OnLevelActorDeleted().RemoveAll(this);

	UObject::BeginDestroy();
}

void UActorLockerManager::SetLockTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem, const bool bInLock, const bool bPropagateToChildren)
{
	const auto Id = GetIdFromTreeItem(InTreeItem);

	if (Id == 0)
	{
		return;
	}

	if (LockedItems.Contains(Id) == bInLock && LockedItems[Id].IsValid())
	{
		return;
	}

	if (bInLock)
	{
		LockedItems.Add(Id, InTreeItem);
	}
	else
	{
		LockedItems.Remove(Id);
	}

	if (const auto ActorItem = InTreeItem.Pin()->CastTo<FActorTreeItem>())
	{
		if (const auto Actor = ActorItem->Actor.Get())
		{
			Actor->SetLockLocation(bInLock);
		}
	}

	if (bPropagateToChildren)
	{
		for (auto& ChildPtr : InTreeItem.Pin()->GetChildren())
		{
			auto Child = ChildPtr.Pin();
			if (Child.IsValid())
			{
				SetLockTreeItem(Child, bInLock);
			}
		}
	}

	CheckParentLock(InTreeItem);
}

void UActorLockerManager::UnlockById(const uint32 InId)
{
	if (!LockedItems.Contains(InId))
	{
		return;
	}

	const auto Item = LockedItems[InId];
	if (Item.IsValid())
	{
		SetLockTreeItem(Item, false);
	}
	else
	{
		LockedItems.Remove(InId);
	}
}

uint32 UActorLockerManager::GetIdFromTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const
{
	if (!InTreeItem.IsValid())
	{
		return 0;
	}

	const auto TreeItemPtr = InTreeItem.Pin();
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

	UE_LOG(LogActorLockerManager, Error, TEXT("Failed to get id from tree item: %s"), *TreeItemPtr->GetDisplayString());
	return 0;
}

bool UActorLockerManager::IsActorLocked(AActor* InActor) const
{
	if (!IsValid(InActor))
	{
		return false;
	}

	const auto ActorId = InActor->GetUniqueID();
	return LockedItems.Contains(ActorId);
}

bool UActorLockerManager::IsItemLocked(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const
{
	const auto Id = GetIdFromTreeItem(InTreeItem);
	return Id == 0 ? false : LockedItems.Contains(Id);
}

void UActorLockerManager::CheckParentLock(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem)
{
	const auto ParentTreeItem = InTreeItem.Pin()->GetParent();
	if (!ParentTreeItem.IsValid() || ParentTreeItem->IsA<FActorTreeItem>())
	{
		return;
	}

	const auto bLocked = IsItemLocked(ParentTreeItem);
	const auto bAllChildrenLocked = !IsAnyChildUnlocked(ParentTreeItem);

	if (bLocked != bAllChildrenLocked)
	{
		SetLockTreeItem(ParentTreeItem, bAllChildrenLocked, false);
	}
}

bool UActorLockerManager::IsAnyChildUnlocked(const TWeakPtr<ISceneOutlinerTreeItem>& InParentTreeItem) const
{
	if (!InParentTreeItem.IsValid() || InParentTreeItem.Pin()->IsA<FActorTreeItem>())
	{
		return false;
	}
	
	for (auto& ChildPtr : InParentTreeItem.Pin()->GetChildren())
	{
		auto Child = ChildPtr.Pin();
		if (Child.IsValid())
		{
			if (!IsItemLocked(Child))
			{
				return true;
			}
		}
	}

	return false;
}

void UActorLockerManager::UpdateLockState()
{
	TSet<TWeakPtr<ISceneOutlinerTreeItem>> CheckedParents;
	for (auto It = LockedItems.CreateConstIterator(); It; ++It)
	{
		const auto Item = It.Value();
		if (!Item.IsValid())
		{
			continue;
		}
		
		const auto ItemPtr = Item.Pin();
		const auto Children = ItemPtr->GetChildren();
		const auto ParentItemPtr = ItemPtr->GetParent();
			
		if (!ParentItemPtr.IsValid() || CheckedParents.Contains(ParentItemPtr))
		{
			continue;
		}
			
		CheckedParents.Add(ParentItemPtr);
		CheckParentLock(ItemPtr);
	}
}

TSet<AActor*> UActorLockerManager::GetLockedActors() const
{
	TSet<AActor*> LockedActors;

	for (auto It = LockedItems.CreateConstIterator(); It; ++It)
	{
		const auto Item = It.Value();
		if (Item.IsValid())
		{
			const auto TreeItemPtr = Item.Pin();
			if (const auto ActorTreeItem = TreeItemPtr->CastTo<FActorTreeItem>())
			{
				if (ActorTreeItem->Actor.IsValid())
				{
					LockedActors.Add(ActorTreeItem->Actor.Get());
				}
				else
				{
					UE_LOG(LogActorLockerManager, Error, TEXT("Failed to get actor from tree item: %s"), *TreeItemPtr->GetDisplayString());
				}
			}
		}
		else
		{
			UE_LOG(LogActorLockerManager, Error, TEXT("Item with key %u is invalid. Did we forget to remove it?"), It.Key());
		}
	}

	return LockedActors;
}

UActorLockerManager* UActorLockerManager::GetActorLockerManager()
{
	const auto& ActorLockerModule = FModuleManager::GetModuleChecked<FActorLockerModule>("ActorLocker");
	return ActorLockerModule.GetActorLockerManager();
}

void UActorLockerManager::OnActorSelected(UObject* InObject)
{
	const auto Actor = Cast<AActor>(InObject);
	if (!IsValid(Actor))
	{
		return;
	}

	if (IsActorLocked(Actor))
	{
		GEditor->SelectActor(Actor, false, true, false, true);
	}
}

void UActorLockerManager::OnActorDeleted(AActor* InActor)
{
	const auto Id = InActor->GetUniqueID();
	LockedItems.Remove(Id);
}

void UActorLockerManager::OnSelectionChanged(UObject* Object)
{
	if (!IsValid(Object))
	{
		return;
	}

	auto Selection = Cast<USelection>(Object);
	if (!IsValid(Selection))
	{
		return;
	}

	TArray<AActor*> SelectedActors;
	Selection->GetSelectedObjects(SelectedActors);

	if (SelectedActors.Num() == 0)
	{
		return;
	}

	for (const auto SelectedActor : SelectedActors)
	{
		const auto Level = SelectedActor->GetLevel();
		const auto bDefaultBrush = IsValid(Level) ? Level->GetDefaultBrush() == SelectedActor : false;

		if (IsActorLocked(SelectedActor) || bDefaultBrush)
		{
			Selection->Deselect(SelectedActor);
		}
	}

	UE_LOG(LogActorLockerManager, Log, TEXT("OnSelectionChanged: %s"), *Object->GetName());
}

void UActorLockerManager::OnPostTick(float InDeltaTime)
{
	FSlateApplication::Get().OnPostTick().RemoveAll(this);
	UpdateLockState();
}

void UActorLockerManager::OnInputEvent(const FSlateDebuggingInputEventArgs& SlateDebuggingInputEventArgs)
{
	if (SlateDebuggingInputEventArgs.InputEventType != ESlateDebuggingInputEvent::DragDrop)
	{
		return;
	}

	const auto Widget = SlateDebuggingInputEventArgs.HandlerWidget;
	if (!Widget.IsValid())
	{
		return;
	}

	if (auto OutlinerTreeRow = StaticCastSharedPtr<SSceneOutlinerTreeRow>(Widget))
	{
		if (!FSlateApplication::Get().OnPostTick().IsBoundToObject(this))
		{
			FSlateApplication::Get().OnPostTick().AddUObject(this, &UActorLockerManager::OnPostTick);
		}
	}
}
