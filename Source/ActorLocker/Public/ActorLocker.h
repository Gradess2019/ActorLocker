// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define OLDER_THAN_UE_5_1 (ENGINE_MAJOR_VERSION < 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 1))

class FActorLockerMenuExtender;
class UActorLockerManager;

class FActorLockerModule : public IModuleInterface
{
private:
	TWeakObjectPtr<UActorLockerManager> ActorLockerManager;
	TSharedPtr<FActorLockerMenuExtender> MenuExtender;

#if OLDER_THAN_UE_5_1
	FDelegateHandle OnPreWorldInitializationHandle;
#endif
	
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

	TWeakObjectPtr<UActorLockerManager> GetActorLockerManager() const { return ActorLockerManager; }

protected:
	void CreateActorLockerManager(const FString& Filename, bool bAsTemplate);
	void CreateActorLockerManager();
	void CreateActorLockerMenuExtender();
	
	void DestroyActorLockerMenuExtender();
};
