// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/InputChord.h"
#include "UObject/Object.h"
#include "ActorLockerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = ActorLockerSettings)
class ACTORLOCKER_API UActorLockerSettings : public UObject
{
	GENERATED_BODY()

public:
	UActorLockerSettings();

#pragma region General
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | General")
	bool bDeselectActorOnLock = false;

	// if true the locked actors will be marked by a tag
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | General")
	bool bSaveLockedState = true;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | General", meta = (EditCondition = "bSaveLockedState"))
	FName LockedTag;
#pragma endregion General

#pragma region Hotkeys
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Hotkeys")
	FInputChord LockObject;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Hotkeys")
	FInputChord UnlockObject;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Hotkeys")
	FInputChord LockAllObjects;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Hotkeys")
	FInputChord UnlockAllObjects;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Hotkeys")
	FInputChord ToggleLockedObjects;
#pragma endregion Hotkeys

#pragma region EditorMode
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Editor Mode")
	bool bSelectLockedActorsInOutliner = true;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker Editor Mode")
	TSet<FName> OutlinerWidgetTypes;
	
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker Editor Mode")
	TSet<FName> MenuWidgetTypes;
	
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker Editor Mode")
	TSet<FName> LockerWidgetTypes;
	
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker Editor Mode")
	TSet<FName> IgnoredWidgetTypes;
#pragma endregion EditorMode

public:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
};
