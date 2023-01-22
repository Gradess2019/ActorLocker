// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorLockerTypes.h"

class FActorLockerMenuExtender;
class UActorLockerManager;

#if OLDER_THAN_UE_5_1
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorLockerManagerCreated, UActorLockerManager*);
#endif


class FActorLockerModule : public IModuleInterface
{
public:
#if OLDER_THAN_UE_5_1
	FOnActorLockerManagerCreated OnActorLockerManagerCreated;
#endif
	
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
