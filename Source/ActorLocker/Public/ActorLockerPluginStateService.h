// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorLockerPluginStateService.generated.h"

struct FPluginReferenceDescriptor;

UCLASS()
class ACTORLOCKER_API UActorLockerPluginStateService : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "ActorLocker|PluginState")
	bool bLastPluginState = false;
	
	TSharedPtr<IPlugin> Plugin;

public:
	virtual void PostInitProperties() override;
	
	//~ Begin FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	//~ End FTickableGameObject Interface
};
