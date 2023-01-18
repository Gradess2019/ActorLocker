// Copyright 2023 Gradess Games. All Rights Reserved.


#include "ActorLockerSettings.h"

UActorLockerSettings::UActorLockerSettings()
{
	LockObject = FInputChord(EKeys::Comma, EModifierKey::Alt);
	UnlockObject = FInputChord(EKeys::Period, EModifierKey::Alt);
	LockAllObjects = FInputChord(EKeys::Comma, EModifierKey::Alt | EModifierKey::Shift);
	UnlockAllObjects = FInputChord(EKeys::Period, EModifierKey::Alt | EModifierKey::Shift);
	ToggleLockedObjects = FInputChord(EKeys::Slash, EModifierKey::Alt);
}
