// Copyright 2023 Gradess Games. All Rights Reserved.

#include "ActorLockerMenuExtender.h"
#include "ActorLockerCommandManager.h"
#include "ActorLockerCommands.h"
#include "ActorLockerManager.h"
#include "ActorLockerStyle.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "ActorLockerMenuExtender"

void FActorLockerMenuExtender::AddLevelViewportMenuExtender()
{
	auto& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	auto& ExtenderDelegates = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();
	ExtenderDelegates.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(this, &FActorLockerMenuExtender::OnExtendLevelEditor));
	MenuExtenderDelegateHandle = ExtenderDelegates.Last().GetHandle();
}

void FActorLockerMenuExtender::RemoveLevelViewportMenuExtender()
{
	if (!MenuExtenderDelegateHandle.IsValid())
	{
		return;
	}

	if (const auto LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor"))
	{
		LevelEditorModule->GetAllLevelViewportContextMenuExtenders().RemoveAll([=](const auto& In) { return In.GetHandle() == MenuExtenderDelegateHandle; });
	}
}

TSharedRef<FExtender> FActorLockerMenuExtender::OnExtendLevelEditor(const TSharedRef<FUICommandList> CommandList, const TArray<AActor*> SelectedActors)
{
	TSharedRef<FExtender> Extender(new FExtender());

	if (SelectedActors.Num() > 0)
	{
		Extender->AddMenuExtension(
			"ActorTypeTools",
			EExtensionHook::After,
			CommandList,
			FMenuExtensionDelegate::CreateRaw(this, &FActorLockerMenuExtender::CreateActorLockerMenu, SelectedActors)
		);
	}

	return Extender;
}

void FActorLockerMenuExtender::CreateActorLockerMenu(FMenuBuilder& MenuBuilder, TArray<AActor*> Actors)
{
	const auto ActorLockerManager = UActorLockerManager::GetActorLockerManager();
	if (!IsValid(ActorLockerManager))
	{
		return;
	}

	const auto& Commands = FActorLockerCommands::Get();

	auto bAllUnlocked = true;
	for (const auto Actor : Actors)
	{
		if (ActorLockerManager->IsActorLocked(Actor))
		{
			bAllUnlocked = false;
			break;
		}
	}

	if (bAllUnlocked)
	{
		MenuBuilder.AddMenuEntry
		(
			Commands.LockObject,
			TEXT("ActorLocker"),
			LOCTEXT("Lock", "Lock selection"),
			LOCTEXT("Lock_Tooltip", "Lock selected actors"),
			FSlateIcon(FActorLockerStyle::Get().GetStyleSetName(), "SceneOutliner.Lock")
		);
	}
	else
	{
		MenuBuilder.AddMenuEntry
		(
			Commands.UnlockObject,
			TEXT("ActorLocker"),
			LOCTEXT("Unlock", "Unlock selection"),
			LOCTEXT("Unlock_Tooltip", "Unlock selected actors"),
			FSlateIcon(FActorLockerStyle::Get().GetStyleSetName(), "SceneOutliner.Unlock")
		);
	}

	MenuBuilder.AddMenuEntry(
		Commands.ToggleLockedObjects,
		TEXT("ActorLocker"),
		LOCTEXT("ToggleLock", "Toggle locked actors state"),
		LOCTEXT("ToggleLock_Tooltip", "Toggle lock for locked actors"),
		FSlateIcon(FActorLockerStyle::Get().GetStyleSetName(), "SceneOutliner.Lock")
	);
}

#undef LOCTEXT_NAMESPACE
