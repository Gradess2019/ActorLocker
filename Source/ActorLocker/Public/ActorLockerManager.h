// Copyright Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	TMap<uint32, TWeakPtr<ISceneOutlinerTreeItem>> LockedItems;

public:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	void SetLockTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem, const bool bInLock);
	void UnlockById(const uint32 InId);
	
	uint32 GetIdFromTreeItem(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const;
	
	UFUNCTION(BlueprintPure, Category = "ActorLocker")
	bool IsActorLocked(AActor* InActor) const;
	bool IsItemLocked(const TWeakPtr<ISceneOutlinerTreeItem>& InTreeItem) const;

	UFUNCTION(BlueprintPure, Category = "ActorLocker")
	TSet<AActor*> GetLockedActors() const;

	UFUNCTION(BlueprintPure, Category = "ActorLocker")
	static UActorLockerManager* GetActorLockerManager();

protected:
	UFUNCTION()
	void OnActorSelected(UObject* InObject);

	UFUNCTION()
	void OnActorDeleted(AActor* InActor);

};


