// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActorLocker.h"
#include "ActorLockerManager.h"
#include "ActorLockerStyle.h"
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

	FEditorDelegates::OnMapOpened.AddRaw(this, &FActorLockerModule::CreateActorLockerManager);
}

void FActorLockerModule::ShutdownModule()
{
	FSceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>("SceneOutliner");
	SceneOutlinerModule.UnRegisterColumnType<FSceneOutlinerActorLocker>();

	FActorLockerStyle::Shutdown();
}

void FActorLockerModule::CreateActorLockerManager(const FString& Filename, bool bAsTemplate)
{
	if (ActorLockerManager.IsValid())
	{
		ActorLockerManager->RemoveFromRoot();
		ActorLockerManager->MarkAsGarbage();
	}
	
	ActorLockerManager = NewObject<UActorLockerManager>();
	ActorLockerManager->AddToRoot();
}

IMPLEMENT_MODULE(FActorLockerModule, ActorLocker)
