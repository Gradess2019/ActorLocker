// Copyright Gradess Games. All Rights Reserved.

#include "ActorLockerCommands.h"

#define LOCTEXT_NAMESPACE "ActorLockerPlugin"

void FActorLockerCommands::RegisterCommands()
{
	UI_COMMAND(LockObject, "LockObject", "Lock selected object", EUserInterfaceActionType::Button, FInputChord(EKeys::Comma, EModifierKey::Alt));
	UI_COMMAND(UnlockObject, "UnlockObject", "Unlock selected object", EUserInterfaceActionType::Button, FInputChord(EKeys::Period, EModifierKey::Alt));
	UI_COMMAND(LockAllObjects, "LockAllObjects", "Lock all selected objects", EUserInterfaceActionType::Button, FInputChord(EKeys::Comma, EModifierKey::Alt | EModifierKey::Shift));
	UI_COMMAND(UnlockAllObjects, "UnlockAllObjects", "Unlock all selected objects", EUserInterfaceActionType::Button, FInputChord(EKeys::Period, EModifierKey::Alt | EModifierKey::Shift));
	UI_COMMAND(ToggleLockedObjects, "ToggleLockedObjects", "Toggle state of locked objects", EUserInterfaceActionType::Button, FInputChord(EKeys::M, EModifierKey::Alt));
}

#undef LOCTEXT_NAMESPACE
