// Copyright 2023 Gradess Games. All Rights Reserved.

#include "ActorLockerCommands.h"
#include "ActorLockerSettings.h"

#define LOCTEXT_NAMESPACE "ActorLockerPlugin"

void FActorLockerCommands::RegisterCommands()
{
	const auto ActorLockerSettings = GetDefault<UActorLockerSettings>();
	UI_COMMAND(LockObject, "LockObject", "Lock selected object", EUserInterfaceActionType::Button, ActorLockerSettings->LockObject);
	UI_COMMAND(UnlockObject, "UnlockObject", "Unlock selected object", EUserInterfaceActionType::Button, ActorLockerSettings->UnlockObject);
	UI_COMMAND(LockAllObjects, "LockAllObjects", "Lock all selected objects", EUserInterfaceActionType::Button, ActorLockerSettings->LockAllObjects);
	UI_COMMAND(UnlockAllObjects, "UnlockAllObjects", "Unlock all selected objects", EUserInterfaceActionType::Button, ActorLockerSettings->UnlockAllObjects);
	UI_COMMAND(ToggleLockedObjects, "ToggleLockedObjects", "Toggle state of locked objects", EUserInterfaceActionType::Button, ActorLockerSettings->ToggleLockedObjects);
}

#undef LOCTEXT_NAMESPACE
