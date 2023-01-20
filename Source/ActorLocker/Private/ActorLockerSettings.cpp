// Copyright Gradess Games. All Rights Reserved.


#include "ActorLockerSettings.h"
#include "ActorLockerEditorMode.h"
#include "EditorModeManager.h"

UActorLockerSettings::UActorLockerSettings()
{
	LockObject = FInputChord(EKeys::Comma, EModifierKey::Alt);
	UnlockObject = FInputChord(EKeys::Period, EModifierKey::Alt);
	LockAllObjects = FInputChord(EKeys::Comma, EModifierKey::Alt | EModifierKey::Shift);
	UnlockAllObjects = FInputChord(EKeys::Period, EModifierKey::Alt | EModifierKey::Shift);
	ToggleLockedObjects = FInputChord(EKeys::Slash, EModifierKey::Alt);
}

void UActorLockerSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	if (GET_MEMBER_NAME_CHECKED(UActorLockerSettings, bSelectLockedActorsInOutliner) == PropertyChangedEvent.Property->GetFName())
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
	UObject::PostEditChangeChainProperty(PropertyChangedEvent);
}
