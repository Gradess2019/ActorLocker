// Copyright 2023 Gradess Games. All Rights Reserved.

#include "ActorLockerCommandManager.h"
#include "ActorLockerCommands.h"
#include "ActorLockerManager.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "Selection.h"

TSharedRef<FUICommandList> UActorLockerCommandManager::RegisterCommands()
{
	FActorLockerCommands::Register();

	const auto& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	const auto& Commands = FActorLockerCommands::Get();
	const auto CommandList = LevelEditorModule.GetGlobalLevelEditorActions();

	CommandList->MapAction(
		Commands.LockObject,
		FExecuteAction::CreateStatic(&UActorLockerCommandManager::LockObject),
		FCanExecuteAction::CreateStatic(&UActorLockerCommandManager::CanLockObject)
	);

	CommandList->MapAction(
		Commands.UnlockObject,
		FExecuteAction::CreateStatic(&UActorLockerCommandManager::UnlockObject),
		FCanExecuteAction::CreateStatic(&UActorLockerCommandManager::CanUnlockObject)
	);

	CommandList->MapAction(
		Commands.LockAllObjects,
		FExecuteAction::CreateStatic(&UActorLockerCommandManager::LockAllObjects),
		FCanExecuteAction::CreateStatic(&UActorLockerCommandManager::CanLockAllObjects)
	);

	CommandList->MapAction(
		Commands.UnlockAllObjects,
		FExecuteAction::CreateStatic(&UActorLockerCommandManager::UnlockAllObjects),
		FCanExecuteAction::CreateStatic(&UActorLockerCommandManager::CanUnlockAllObjects)
	);

	CommandList->MapAction(
		Commands.ToggleLockedObjects,
		FExecuteAction::CreateStatic(&UActorLockerCommandManager::ToggleLockedObjects),
		FCanExecuteAction::CreateStatic(&UActorLockerCommandManager::CanToggleLockedObjects)
	);

	return CommandList;
}

void UActorLockerCommandManager::UnregisterCommands()
{
	FActorLockerCommands::Unregister();
}

void UActorLockerCommandManager::LockObject()
{
	GEditor->BeginTransaction(FText::FromString(TEXT("Lock object")));
	SetLockActors(true);
	GEditor->EndTransaction();
}

void UActorLockerCommandManager::UnlockObject()
{
	GEditor->BeginTransaction(FText::FromString(TEXT("Unlock object")));
	SetLockActors(false);
	GEditor->EndTransaction();
}

void UActorLockerCommandManager::LockAllObjects()
{
	GEditor->BeginTransaction(FText::FromString(TEXT("Lock all objects")));
	SetLockAllActors(true);
	GEditor->EndTransaction();
}

void UActorLockerCommandManager::UnlockAllObjects()
{
	GEditor->BeginTransaction(FText::FromString(TEXT("Unlock all objects")));
	SetLockAllActors(false);
	GEditor->EndTransaction();
}

void UActorLockerCommandManager::ToggleLockedObjects()
{
	GEditor->BeginTransaction(FText::FromString(TEXT("Toggle locked objects")));

	const auto ActorLockerManager = UActorLockerManager::GetActorLockerManager();

	SaveToTransactionBuffer(ActorLockerManager, false);
	ActorLockerManager->ToggleLockedActors();
	
	GEditor->EndTransaction();
}

bool UActorLockerCommandManager::CanLockObject()
{
	return IsValidActorLockerManager();
}

bool UActorLockerCommandManager::CanUnlockObject()
{
	return IsValidActorLockerManager();
}

bool UActorLockerCommandManager::CanLockAllObjects()
{
	return IsValidActorLockerManager() && IsValidEditorWorld();
}

bool UActorLockerCommandManager::CanUnlockAllObjects()
{
	return IsValidActorLockerManager() && IsValidEditorWorld();
}

bool UActorLockerCommandManager::CanToggleLockedObjects()
{
	return IsValidActorLockerManager();
}

void UActorLockerCommandManager::SetLockActors(const bool bInLock)
{
	auto ActorLockerManager = UActorLockerManager::GetActorLockerManager();
	SaveToTransactionBuffer(ActorLockerManager, false);

	if (const auto Selection = GEditor->GetSelectedActors())
	{
		TArray<AActor*> SelectedActors;
		Selection->GetSelectedObjects<AActor>(SelectedActors);

		for (const auto SelectedActor : SelectedActors)
		{
			ActorLockerManager->SetLockActor(SelectedActor, bInLock);
		}
	}
}

void UActorLockerCommandManager::SetLockAllActors(const bool bInLock)
{
	const auto ActorLockerManager = UActorLockerManager::GetActorLockerManager();
	const auto World = GEditor->GetEditorWorldContext().World();

	SaveToTransactionBuffer(ActorLockerManager, false);
	
	for (TActorIterator<AActor> ActorIterator(World); ActorIterator; ++ActorIterator)
	{
		ActorLockerManager->SetLockActor(*ActorIterator, bInLock);
	}
}

bool UActorLockerCommandManager::IsValidActorLockerManager()
{
	const auto ActorLockerManager = UActorLockerManager::GetActorLockerManager();
	return IsValid(ActorLockerManager);
}

bool UActorLockerCommandManager::IsValidEditorWorld()
{
	const auto World = GEditor->GetEditorWorldContext().World();
	return IsValid(World);
}
