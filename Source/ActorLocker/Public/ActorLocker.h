// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UActorLockerManager;

class FActorLockerModule : public IModuleInterface
{
private:
	TWeakObjectPtr<UActorLockerManager> ActorLockerManager;
	
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

	UActorLockerManager* GetActorLockerManager() const { return ActorLockerManager.Get(); }

protected:
	void CreateActorLockerManager(const FString& Filename, bool bAsTemplate);
};
