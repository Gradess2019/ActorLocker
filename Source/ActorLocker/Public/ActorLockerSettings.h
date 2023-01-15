// Copyright Gradess Games. All Rights Reserved.

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
	
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker")
	FInputChord LockObject;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker")
	FInputChord UnlockObject;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker")
	FInputChord LockAllObjects;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker")
	FInputChord UnlockAllObjects;

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Actor Locker")
	FInputChord ToggleLockedObjects;
};
