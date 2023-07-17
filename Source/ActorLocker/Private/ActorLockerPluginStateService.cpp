// Copyright 2023 Gradess Games. All Rights Reserved.


#include "ActorLockerPluginStateService.h"
#include "ActorLockerSettings.h"
#include "IPythonScriptPlugin.h"
#include "LevelEditor.h"
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
		const auto& Message = GetDefault<UActorLockerSettings>()->PluginStateServiceWarningMessage;
		const auto Answer = FMessageDialog::Open(EAppMsgType::YesNo, Message);
		if (Answer == EAppReturnType::Yes)
		{
			const auto& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
			const auto& LevelEditorTab = LevelEditorModule.GetLevelEditorTab();
			LevelEditorTab->ActivateInParent(ETabActivationCause::SetDirectly);

			const auto& ScriptFileName = GetDefault<UActorLockerSettings>()->ScriptFileName;
			IPythonScriptPlugin::Get()->ExecPythonCommand(*ScriptFileName);
		}
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
