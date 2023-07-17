// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorLockerPythonCommand.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ACTORLOCKER_API UActorLockerPythonCommand : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "ActorLockerPythonCommand")
	void AddObjectToRoot();

	UFUNCTION(BlueprintCallable, Category = "ActorLockerPythonCommand")
	void RemoveObjectFromRoot();

	UFUNCTION(BlueprintCallable, Category="ActorLockerPythonCommand", meta=(DefaultToSelf = "Object", AdvancedDisplay="InitialStartDelay"))
	static void SetEditorTimer(UObject* Object, FString FunctionName, float Time, bool bLooping = false, float InitialStartDelay = -1);
};
