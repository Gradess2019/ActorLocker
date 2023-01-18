// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorLockerStyle.h"
#include "Framework/Commands/Commands.h"
#include "Styling/ISlateStyle.h"

/**
 * 
 */
class ACTORLOCKER_API FActorLockerCommands : public TCommands<FActorLockerCommands>
{
	
public:
	FActorLockerCommands() : TCommands<FActorLockerCommands>
	(
		"ActorLocker",
		NSLOCTEXT("Contexts", "ActorLocker", "Actor Locker"),
		NAME_None,
		FActorLockerStyle::Get().GetStyleSetName()
	)
	{
	}

	//~ Begin TCommands Interface
	virtual void RegisterCommands() override;
	//~ End TCommands Interface

public:
	TSharedPtr<FUICommandInfo> LockObject;
	TSharedPtr<FUICommandInfo> UnlockObject;
	TSharedPtr<FUICommandInfo> LockAllObjects;
	TSharedPtr<FUICommandInfo> UnlockAllObjects;
	TSharedPtr<FUICommandInfo> ToggleLockedObjects;
};
