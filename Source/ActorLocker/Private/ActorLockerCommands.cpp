// Copyright Gradess Games. All Rights Reserved.

#include "ActorLockerCommands.h"

#define LOCTEXT_NAMESPACE "ActorLockerPlugin"

void FActorLockerCommands::RegisterCommands()
{
	UI_COMMAND(LockObject, "LockObject", "Lock selected object", EUserInterfaceActionType::Button, FInputChord(EKeys::L, EModifierKey::Alt));
	UI_COMMAND(UnlockObject, "UnlockObject", "Unlock selected object", EUserInterfaceActionType::Button, FInputChord(EKeys::U, EModifierKey::Alt));
	UI_COMMAND(LockAllObjects, "LockAllObjects", "Lock all selected objects", EUserInterfaceActionType::Button, FInputChord(EKeys::L, EModifierKey::Alt | EModifierKey::Shift));
	UI_COMMAND(UnlockAllObjects, "UnlockAllObjects", "Unlock all selected objects", EUserInterfaceActionType::Button, FInputChord(EKeys::U, EModifierKey::Alt | EModifierKey::Shift));
	UI_COMMAND(ToggleLockedObjects, "ToggleLockedObjects", "Toggle state of locked objects", EUserInterfaceActionType::Button, FInputChord(EKeys::T, EModifierKey::Control));
}

#undef LOCTEXT_NAMESPACE
