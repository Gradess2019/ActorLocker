// Copyright 2023 Gradess Games. All Rights Reserved.

#include "ActorLocker.h"
#include "ActorLockerCommandManager.h"
#include "ActorLockerManager.h"
#include "ActorLockerMenuExtender.h"
#include "ActorLockerPluginStateService.h"
#include "ActorLockerSettings.h"
#include "ActorLockerStyle.h"
#include "ISettingsModule.h"
#include "SceneOutlinerActorLocker.h"
#include "SceneOutlinerModule.h"
#include "Selection.h"

void FActorLockerModule::StartupModule()
{
	FActorLockerStyle::Initialize();
	FActorLockerStyle::ReloadTextures();
	
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>("SceneOutliner");

	const auto ColumnVisibility = ESceneOutlinerColumnVisibility::Visible;
	const auto PriorityIndex = 9;
	const auto Factory = FCreateSceneOutlinerColumn::CreateLambda([](ISceneOutliner& SceneOutliner){ return MakeShareable(new FSceneOutlinerActorLocker(SceneOutliner)); });
	const auto bCanBeHidden = false;
	const auto FillSize = TOptional<float>();
	const auto ColumnLabel = FSceneOutlinerActorLocker::Lock_Localized();
	const auto ColumnInfo = FSceneOutlinerColumnInfo(ColumnVisibility, PriorityIndex, Factory, bCanBeHidden, FillSize, ColumnLabel);
	SceneOutlinerModule.RegisterDefaultColumnType<FSceneOutlinerActorLocker>(ColumnInfo);

	if (const auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "ActorLocker",
			NSLOCTEXT("ActorLocker", "ActorLockerSettingsDisplayName", "Actor Locker"),
			NSLOCTEXT("ActorLocker", "ActorLockerSettingsDescription", "Configure the Actor Locker plugin (needs restart)"),
			GetMutableDefault<UActorLockerSettings>()
		);
	}
	
	UActorLockerCommandManager::RegisterCommands();

	CreateActorLockerMenuExtender();

#if OLDER_THAN_UE_5_1
	OnPreWorldInitializationHandle = FWorldDelegates::OnPreWorldInitialization.AddLambda([this] (UWorld* World, const UWorld::InitializationValues IVS)
	{
		CreateActorLockerManager();
		FWorldDelegates::OnPreWorldInitialization.Remove(OnPreWorldInitializationHandle);
		FEditorDelegates::OnMapOpened.AddRaw(this, &FActorLockerModule::OnMapOpened);
	});
#else
	FEditorDelegates::OnMapOpened.AddRaw(this, &FActorLockerModule::OnMapOpened);
#endif
	
}

void FActorLockerModule::ShutdownModule()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>("SceneOutliner");
	SceneOutlinerModule.UnRegisterColumnType<FSceneOutlinerActorLocker>();

	DestroyActorLockerMenuExtender();
	
	UActorLockerCommandManager::UnregisterCommands();

	FActorLockerStyle::Shutdown();
}

TWeakObjectPtr<UActorLockerManager> FActorLockerModule::GetActorLockerManager(const bool bRequired)
{
	if (!ActorLockerManager.IsValid() && bRequired)
	{
		CreateActorLockerManager();
	}

	return ActorLockerManager;
}

void FActorLockerModule::OnMapOpened(const FString& Filename, bool bAsTemplate)
{
	CreateActorLockerManager();

	if (!PluginStateService.IsValid())
	{
		PluginStateService = NewObject<UActorLockerPluginStateService>();
		PluginStateService->AddToRoot();
	}
}

void FActorLockerModule::CreateActorLockerManager()
{
	if (ActorLockerManager.IsValid())
	{
		ActorLockerManager->RemoveFromRoot();
		ActorLockerManager->MarkAsGarbage();
	}
	
	ActorLockerManager = NewObject<UActorLockerManager>();
	ActorLockerManager->AddToRoot();

	OnActorLockerManagerCreated.Broadcast(ActorLockerManager.Get());
}

void FActorLockerModule::CreateActorLockerMenuExtender()
{
	MenuExtender = MakeShareable(new FActorLockerMenuExtender());
	MenuExtender->AddLevelViewportMenuExtender();
}

void FActorLockerModule::DestroyActorLockerMenuExtender()
{
	MenuExtender.Reset();
}

IMPLEMENT_MODULE(FActorLockerModule, ActorLocker)
