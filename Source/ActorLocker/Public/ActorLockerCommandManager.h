// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorLockerCommandManager.generated.h"

class UActorLockerManager;

/**
 * 
 */
UCLASS()
class ACTORLOCKER_API UActorLockerCommandManager : public UObject
{
	GENERATED_BODY()

public:
	static TSharedRef<FUICommandList> RegisterCommands();
	static void UnregisterCommands();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static void LockObject();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static void UnlockObject();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static void LockAllObjects();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static void UnlockAllObjects();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static void ToggleLockedObjects(); 

	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static bool CanLockObject();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static bool CanUnlockObject();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static bool CanLockAllObjects();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static bool CanUnlockAllObjects();
	
	UFUNCTION(BlueprintCallable, Category = "Actor Locker Command Manager")
	static bool CanToggleLockedObjects();

protected:
	static void SetLockActors(const bool bInLock);
	static void SetLockAllActors(const bool bInLock);
	
	static bool IsValidActorLockerManager();
	static bool IsValidEditorWorld();
};
