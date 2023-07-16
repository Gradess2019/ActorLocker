// Copyright 2023 Gradess Games. All Rights Reserved.


#include "ActorLockerPluginStateService.h"
#include "ProjectDescriptor.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"


void UActorLockerPluginStateService::PostInitProperties()
{
	UObject::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	auto& PluginManager = IPluginManager::Get();
	Plugin = PluginManager.FindPlugin(TEXT("ActorLocker"));
}

void UActorLockerPluginStateService::Tick(float DeltaTime)
{
	const auto& ProjectManager = IProjectManager::Get();
	const auto* Descriptor = ProjectManager.GetCurrentProject();

	if (!Descriptor)
	{
		return;
	}

	const FPluginReferenceDescriptor* PluginDescriptor = nullptr;
	for (const auto& PluginReferenceDescriptor : Descriptor->Plugins)
	{
		if (PluginReferenceDescriptor.Name == TEXT("ActorLocker"))
		{
			PluginDescriptor = &PluginReferenceDescriptor;
		}
	}
	
	if (!PluginDescriptor)
	{
		if (Plugin.IsValid())
		{
			bLastPluginState = Plugin->IsEnabled();
		}
		
		return;
	}
	
	const auto bEnabled = Plugin.IsValid() && Plugin->IsEnabled() && PluginDescriptor->bEnabled;
	const auto bStateChanged = bEnabled != bLastPluginState;

	if (bStateChanged && !bEnabled)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("You've just disabled ActorLocker plugin. Make sure that you don't have locked actors on all your maps, otherwise you will need to uncheck \"Lock Actor Movement\" manually after the restart.")));
	}

	bLastPluginState = bEnabled;
}

bool UActorLockerPluginStateService::IsTickableWhenPaused() const
{
	return true;
}

bool UActorLockerPluginStateService::IsTickableInEditor() const
{
	return true;
}

bool UActorLockerPluginStateService::IsTickable() const
{
	return Plugin.IsValid();
}

TStatId UActorLockerPluginStateService::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UActorLockerPluginStateService, STATGROUP_Tickables);
}
