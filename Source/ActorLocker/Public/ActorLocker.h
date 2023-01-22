// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorLockerTypes.h"

class FActorLockerMenuExtender;
class UActorLockerManager;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorLockerManagerCreated, UActorLockerManager*);


class FActorLockerModule : public IModuleInterface
{
public:
	FOnActorLockerManagerCreated OnActorLockerManagerCreated;
	
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

	TWeakObjectPtr<UActorLockerManager> GetActorLockerManager(const bool bRequired = false);

protected:
	void CreateActorLockerManager(const FString& Filename, bool bAsTemplate);
	void CreateActorLockerManager();
	void CreateActorLockerMenuExtender();
	
	void DestroyActorLockerMenuExtender();
};
