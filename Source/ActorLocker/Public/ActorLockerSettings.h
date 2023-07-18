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

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | General", meta = (ConfigRestartRequired = true))
	TSet<FName> CompatibleModes;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | General")
	bool bDeveloperMode = false;
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

#pragma region PluginStateService
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Plugin State Service", meta = (EditCondition = "bDeveloperMode", EditConditionHides))
	FText PluginStateServiceWarningMessage;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Plugin State Service", meta = (EditCondition = "bDeveloperMode", EditConditionHides))
	FString ScriptFileName;
	
	// Checks the plugin state (enabled / disabled) every X seconds to warn user that he need to unlock all actors before disabling the plugin
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker | Plugin State Service", meta = (EditCondition = "bDeveloperMode", EditConditionHides))
	float CheckPluginStateInterval = 0.05f;
#pragma endregion PluginStateService

public:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
};
