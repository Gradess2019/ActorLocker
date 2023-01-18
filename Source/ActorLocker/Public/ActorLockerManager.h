// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorLockerTypes.h"
#include "UObject/Object.h"
#include "ActorLockerManager.generated.h"

struct ISceneOutlinerTreeItem;
class AActor;

/**
 * 
 */
UCLASS()
class ACTORLOCKER_API UActorLockerManager : public UObject
{
	GENERATED_BODY()

protected:
	TMap<uint32, FLockerTreeItem> Items;
	TSet<uint32> ToggledItems;

public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	virtual void Serialize(FArchive& Ar) override;

	void InitItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem);
	void SetLockTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem, const bool bInLock, const bool bPropagateToChildren = true);
	void UnlockById(const uint32 InId);

	UFUNCTION(BlueprintCallable, Category = "Actor Locker Manager")
	void SetLockActor(AActor* InActor, const bool bInLock, const bool bPropagateToChildren = true);

	// Toggling lock state for locked actors (useful for quick change for locked actors)
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Manager")
	void ToggleLockedActors();
	
	UFUNCTION(BlueprintPure, Category = "Actor Locker Manager")
	bool IsActorLocked(AActor* InActor) const;
	bool IsItemLocked(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const;

	void CheckParentLock(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem);
	bool IsAnyChildUnlocked(const TWeakPtr<ISceneOutlinerTreeItem>& InParentTreeItem) const;
	void UpdateLockState();

	UFUNCTION(BlueprintPure, Category = "Actor Locker Manager")
	TSet<AActor*> GetLockedActors() const;

	UFUNCTION(BlueprintPure, Category = "Actor Locker Manager")
	static UActorLockerManager* GetActorLockerManager();

protected:
	UFUNCTION()
	void OnActorSelected(UObject* InObject);

	UFUNCTION()
	void OnActorDeleted(AActor* InActor);

	UFUNCTION()
	void OnSelectionChanged(UObject* Object);

	void OnPostTick(float InDeltaTime);
	void OnInputEvent(const FSlateDebuggingInputEventArgs& SlateDebuggingInputEventArgs);
};


