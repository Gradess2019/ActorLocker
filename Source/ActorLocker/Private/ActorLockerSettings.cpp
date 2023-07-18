// Copyright 2023 Gradess Games. All Rights Reserved.


#include "ActorLockerSettings.h"
#include "ActorLockerEditorMode.h"
#include "EditorModeManager.h"
#include "EditorModes.h"

UActorLockerSettings::UActorLockerSettings()
{
	LockedTag = TEXT("ActorLocked");
	CompatibleModes = {
		FBuiltinEditorModes::EM_None,
		FBuiltinEditorModes::EM_Default,
		FBuiltinEditorModes::EM_MeshPaint,
		TEXT("EM_ModelingToolsEditorMode"),
		TEXT("EM_FractureEditorMode"),
		TEXT("EM_Geometry"),
	};

	LockObject = FInputChord(EKeys::Comma, EModifierKey::Alt);
	UnlockObject = FInputChord(EKeys::Period, EModifierKey::Alt);
	LockAllObjects = FInputChord(EKeys::Comma, EModifierKey::Alt | EModifierKey::Shift);
	UnlockAllObjects = FInputChord(EKeys::Period, EModifierKey::Alt | EModifierKey::Shift);
	ToggleLockedObjects = FInputChord(EKeys::Slash, EModifierKey::Alt);

	OutlinerWidgetTypes = {"SSceneOutlinerTreeRow"};
	MenuWidgetTypes = {"SMenuEntryButton"};
	LockerWidgetTypes = {"SLockWidget"};
	IgnoredWidgetTypes = {"SActorDetails"};

	PluginStateServiceWarningMessage = FText::FromString(TEXT("Warning: ActorLocker plugin has been disabled. All locked actors will remain in their locked position after restarting. To manually unlock them, go to Context Menu -> \"Transform\" -> \"Lock Actor Movement\". Run a Python script to unlock all actors automatically? Note: It may take significant time due to your project's size."));
	ScriptFileName = TEXT("unlock_all_actors.py");
}

void UActorLockerSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	const auto PropertyName = PropertyChangedEvent.Property->GetFName();
	if (GET_MEMBER_NAME_CHECKED(UActorLockerSettings, bSelectLockedActorsInOutliner) == PropertyName)
	{
		FEditorModeTools& Tools = GLevelEditorModeTools();
		const auto Mode = Cast<UActorLockerEditorMode>(Tools.GetActiveScriptableMode(UActorLockerEditorMode::EM_ActorLockerEditorModeId));

		if (bSelectLockedActorsInOutliner)
		{
			Mode->RegisterEvent();
		}
		else
		{
			Mode->UnregisterEvent();
		}
	}

	if (GET_MEMBER_NAME_CHECKED(UActorLockerSettings, OutlinerWidgetTypes) == PropertyName ||
		GET_MEMBER_NAME_CHECKED(UActorLockerSettings, MenuWidgetTypes) == PropertyName ||
		GET_MEMBER_NAME_CHECKED(UActorLockerSettings, LockerWidgetTypes) == PropertyName ||
		GET_MEMBER_NAME_CHECKED(UActorLockerSettings, IgnoredWidgetTypes) == PropertyName)
	{
		FEditorModeTools& Tools = GLevelEditorModeTools();
		const auto Mode = Cast<UActorLockerEditorMode>(Tools.GetActiveScriptableMode(UActorLockerEditorMode::EM_ActorLockerEditorModeId));
		Mode->UpdateWidgetTypes();
	}

	UObject::PostEditChangeChainProperty(PropertyChangedEvent);
}
