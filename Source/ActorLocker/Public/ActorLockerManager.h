// Copyright Gradess Games. All Rights Reserved.

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

public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	void InitItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem);
	void SetLockTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem, const bool bInLock, const bool bPropagateToChildren = true);
	void UnlockById(const uint32 InId);
	
	UFUNCTION(BlueprintPure, Category = "ActorLocker")
	bool IsActorLocked(AActor* InActor) const;
	bool IsItemLocked(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const;

	void CheckParentLock(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem);
	bool IsAnyChildUnlocked(const TWeakPtr<ISceneOutlinerTreeItem>& InParentTreeItem) const;
	void UpdateLockState();

	UFUNCTION(BlueprintPure, Category = "ActorLocker")
	TSet<AActor*> GetLockedActors() const;

	UFUNCTION(BlueprintPure, Category = "ActorLocker")
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


