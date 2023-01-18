// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACTORLOCKER_API FActorLockerMenuExtender
{
protected:
	FDelegateHandle MenuExtenderDelegateHandle;
	
public:
	void AddLevelViewportMenuExtender();
	void RemoveLevelViewportMenuExtender();

protected:
	TSharedRef<FExtender> OnExtendLevelEditor(const TSharedRef<FUICommandList> CommandList, const TArray<AActor*> SelectedActors);
	void CreateActorLockerMenu(FMenuBuilder& MenuBuilder, TArray<AActor*> Actors);
};
