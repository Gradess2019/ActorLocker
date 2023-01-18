// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FActorLockerMenuExtender;
class UActorLockerManager;

class FActorLockerModule : public IModuleInterface
{
private:
	TWeakObjectPtr<UActorLockerManager> ActorLockerManager;
	TSharedPtr<FActorLockerMenuExtender> MenuExtender;
	
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

	TWeakObjectPtr<UActorLockerManager> GetActorLockerManager() const { return ActorLockerManager; }

protected:
	void CreateActorLockerManager(const FString& Filename, bool bAsTemplate);
	void CreateActorLockerMenuExtender();
	
	void DestroyActorLockerMenuExtender();
};
