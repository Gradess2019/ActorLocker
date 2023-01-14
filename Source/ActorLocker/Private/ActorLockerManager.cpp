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

void UActorLockerManager::InitItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem)
{
	const auto Id = FLockerTreeItem::GetId(InTreeItem);

	if (Items.Contains(Id))
	{
		if (!Items[Id].IsValid())
		{
			FLockerTreeItem NewItem(InTreeItem);
			const auto& OutdatedItem = Items[Id];
			NewItem.bLocked = OutdatedItem.bLocked;
			Items.Add(Id, NewItem);
		}

		return;
	}

	Items.Add(Id, FLockerTreeItem(InTreeItem));
}

void UActorLockerManager::SetLockTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem, const bool bInLock, const bool bPropagateToChildren)
{
	const auto Id = FLockerTreeItem::GetId(InTreeItem);

	if (Id == 0)
	{
		return;
	}

	if (!Items.Contains(Id) || !Items[Id].IsValid())
	{
		InitItem(InTreeItem);
	}

	if (Items[Id].bLocked == bInLock)
	{
		return;
	}

	Items[Id].bLocked = bInLock;

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
	if (!Items.Contains(InId))
	{
		return;
	}

	const auto Item = Items[InId];
	SetLockTreeItem(Item, false);
}

bool UActorLockerManager::IsActorLocked(AActor* InActor) const
{
	if (!IsValid(InActor))
	{
		return false;
	}

	const auto ActorId = InActor->GetUniqueID();
	return Items.Contains(ActorId) && Items[ActorId].bLocked;
}

bool UActorLockerManager::IsItemLocked(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const
{
	const auto Id = FLockerTreeItem::GetId(InTreeItem);
	return Id == 0 ? false : Items.Contains(Id) && Items[Id].bLocked;
}

void UActorLockerManager::CheckParentLock(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem)
{
	const auto ParentTreeItem = InTreeItem.Pin()->GetParent();
	if (!ParentTreeItem.IsValid())
	{
		return;
	}

	if (ParentTreeItem->IsA<FActorTreeItem>())
	{
		CheckParentLock(ParentTreeItem);
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
	if (!InParentTreeItem.IsValid())
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

			if (Child->GetChildren().Num() >= 0 && IsAnyChildUnlocked(Child))
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
	for (auto It = Items.CreateConstIterator(); It; ++It)
	{
		const auto Item = It.Value();
		if (!Item.IsValid())
		{
			continue;
		}

		const auto ItemPtr = Item.NativeItem.Pin();
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

	for (auto It = Items.CreateConstIterator(); It; ++It)
	{
		const auto Item = It.Value();
		if (Item.IsValid())
		{
			const auto TreeItemPtr = Item.NativeItem.Pin();
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
	return ActorLockerModule.GetActorLockerManager().Get();
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
	Items.Remove(Id);
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
