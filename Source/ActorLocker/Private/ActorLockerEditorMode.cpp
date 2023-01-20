﻿// Copyright Gradess Games. All Rights Reserved.


#include "ActorLockerEditorMode.h"
#include "ActorLockerManager.h"
#include "ActorLockerSettings.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "SOutlinerTreeView.h"
#include "SSceneOutliner.h"
#include "Stats/StatsMisc.h"


#define LOCTEXT_NAMESPACE "FSampleToolsEditorMode"

const FEditorModeID UActorLockerEditorMode::EM_ActorLockerEditorModeId = TEXT("EM_SampleToolsEditorMode");


UActorLockerEditorMode::UActorLockerEditorMode()
{
	Info = FEditorModeInfo(
		EM_ActorLockerEditorModeId,
		LOCTEXT("ActorLockerEditorModeName", "ActorLockerEditorMode"),
		FSlateIcon(),
		false, 0);
	
	OutlinerWidgetTypes = { "SSceneOutlinerTreeRow" };
	LockerWidgetTypes = { "SLockWidget" };
}

void UActorLockerEditorMode::Enter()
{
	UEdMode::Enter();
	
	if (GetDefault<UActorLockerSettings>()->bSelectLockedActorsInOutliner)
	{
		RegisterEvent();
	}
}

void UActorLockerEditorMode::Exit()
{
	if (GetDefault<UActorLockerSettings>()->bSelectLockedActorsInOutliner)
	{
		UnregisterEvent();
	}
	
	Super::Exit();
}

bool UActorLockerEditorMode::IsCompatibleWith(FEditorModeID OtherModeID) const
{
	static const TSet<FName> CompatibleModes = {TEXT("EM_Default")};
	const auto bCompatible = CompatibleModes.Contains(OtherModeID);
	return bCompatible;
}

bool UActorLockerEditorMode::IsSelectionDisallowed(AActor* InActor, bool bInSelection) const
{
	if (bOutlinerInteraction)
	{
		return false;
	}
	
	if (!bInSelection)
	{
		return false;
	}
	
	const auto LockerManager = UActorLockerManager::GetActorLockerManager();
	const auto bLocked = IsValid(LockerManager) && LockerManager->IsActorLocked(InActor);
	const auto bLevelBrushActor = IsValid(InActor) && InActor->GetLevel()->GetDefaultBrush() == InActor;
	const auto bDenied = bLocked || bLevelBrushActor;
	
	return bDenied;
}

void UActorLockerEditorMode::RegisterEvent()
{
	FSlateDebugging::RegisterWidgetInputRoutingEvent(this);
}

void UActorLockerEditorMode::UnregisterEvent()
{
	FSlateDebugging::UnregisterWidgetInputRoutingEvent(this);
}

void UActorLockerEditorMode::OnProcessInput(ESlateDebuggingInputEvent InputEventType, const FInputEvent& Event)
{
	if (InputEventType == ESlateDebuggingInputEvent::MouseButtonDown && Event.IsPointerEvent())
	{
		bOutlinerInteraction = false;
		
		const auto& PointerEvent = StaticCast<const FPointerEvent&>(Event);
		const auto Position = PointerEvent.GetScreenSpacePosition();
		
		auto& SlateApplication = FSlateApplication::Get();
		const auto Windows = SlateApplication.GetInteractiveTopLevelWindows();
		
		const auto& Path = FSlateApplication::Get().LocateWindowUnderMouse(Position, Windows);
		if (!Path.IsValid())
		{
			return;
		}

		const auto& Widgets = Path.Widgets.GetInternalArray();
		for (const auto& ArrangedWidget : Widgets)
		{
			const auto Type = ArrangedWidget.Widget->GetType();
			if (LockerWidgetTypes.Contains(Type))
			{
				bOutlinerInteraction = false;
				break;
			}
			
			if (OutlinerWidgetTypes.Contains(Type))
			{
				bOutlinerInteraction = true;
			}
		}

		if (bOutlinerInteraction)
		{
			return;
		}

		const auto LockerManager = UActorLockerManager::GetActorLockerManager();
		const auto Selection = GEditor->GetSelectedActors();

		TSet<AActor*> ActorsToDeselect;
		for (FSelectionIterator Iter(*Selection); Iter; ++Iter)
		{
			const auto Actor = Cast<AActor>(*Iter);
			if (LockerManager->IsActorLocked(Actor))
			{
				ActorsToDeselect.Add(Actor);
			}
		}

		for (const auto ActorToDeselect : ActorsToDeselect)
		{
			Selection->Deselect(ActorToDeselect);
		}
	}
}

#undef LOCTEXT_NAMESPACE
