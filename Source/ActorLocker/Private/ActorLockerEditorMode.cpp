// Copyright Gradess Games. All Rights Reserved.


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

	OutlinerWidgetTypes = {"SSceneOutlinerTreeRow"};
	MenuWidgetTypes = {"SMenuEntryButton"};
	LockerWidgetTypes = {"SLockWidget"};
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
	const auto Id = InActor->GetUniqueID();
	if (SelectedItems.Contains(Id))
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
		const auto Modifiers = Event.GetModifierKeys();
		if (!Modifiers.IsControlDown())
		{
			SelectedItems.Reset();
		}
		
		const auto WidgetPath = GetWidgetPath(Event);
		
		uint32 ItemId = 0;
		bOutlinerInteraction = IsOutlinerInteraction(WidgetPath, ItemId);

		if (bOutlinerInteraction)
		{
			SelectedItems.Add(ItemId);
		}
		else
		{
			CheckLockedActorsSelection();
		}
	}
}

FWidgetPath UActorLockerEditorMode::GetWidgetPath(const FInputEvent& Event) const
{
	const auto& PointerEvent = StaticCast<const FPointerEvent&>(Event);
	const auto Position = PointerEvent.GetScreenSpacePosition();

	auto& SlateApplication = FSlateApplication::Get();
	const auto Windows = SlateApplication.GetInteractiveTopLevelWindows();

	return FSlateApplication::Get().LocateWindowUnderMouse(Position, Windows);;
}

bool UActorLockerEditorMode::IsOutlinerInteraction(const FWidgetPath& Path, uint32& OutItemId) const
{
	if (!Path.IsValid())
	{
		return false;
	}

	auto bResult = false;
	const auto& Widgets = Path.Widgets.GetInternalArray();
	TSharedPtr<SSceneOutlinerTreeRow> Row = nullptr;
	TSharedPtr<SSceneOutlinerTreeView> Tree = nullptr;

	for (const auto& ArrangedWidget : Widgets)
	{
		const auto Type = ArrangedWidget.Widget->GetType();

		if (bOutlinerInteraction && MenuWidgetTypes.Contains(Type))
		{
			bResult = true;
			break;
		}

		if (LockerWidgetTypes.Contains(Type))
		{
			bResult = false;
			break;
		}

		if (!Row.IsValid() && Type == TEXT("SSceneOutlinerTreeRow"))
		{
			Row = StaticCastSharedRef<SSceneOutlinerTreeRow>(ArrangedWidget.Widget);
		}

		if (!Tree.IsValid() && Type == TEXT("SSceneOutlinerTreeView"))
		{
			Tree = StaticCastSharedRef<SSceneOutlinerTreeView>(ArrangedWidget.Widget);
		}

		if (Row && Tree)
		{
			const TSharedPtr<ISceneOutlinerTreeItem>* Item = Tree->ItemFromWidget(Row.Get());
			const auto Id = FLockerTreeItem::GetId(Item->Get()->AsWeak());
			OutItemId = Id;
			bResult = true;
		}
	}

	return bResult;
}

void UActorLockerEditorMode::CheckLockedActorsSelection() const
{
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

#undef LOCTEXT_NAMESPACE
