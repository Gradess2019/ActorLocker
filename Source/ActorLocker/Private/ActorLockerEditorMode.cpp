// Copyright Gradess Games. All Rights Reserved.


#include "ActorLockerEditorMode.h"
#include "ActorLockerManager.h"
#include "LevelEditor.h"
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

void UActorLockerEditorMode::OnProcessInput(ESlateDebuggingInputEvent InputEventType, const FInputEvent& Event)
{
	if (InputEventType == ESlateDebuggingInputEvent::MouseButtonDown && Event.IsPointerEvent())
	{
		SCOPE_LOG_TIME_FUNC();
		bOutlinerInteraction = false;
		
		auto& SlateApplication = FSlateApplication::Get();
		const auto& Test = StaticCast<const FPointerEvent&>(Event);
		const auto Position = Test.GetScreenSpacePosition();
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
	}
}

void UActorLockerEditorMode::Enter()
{
	UEdMode::Enter();
	FSlateDebugging::RegisterWidgetInputRoutingEvent(this);
}

void UActorLockerEditorMode::Exit()
{
	FSlateDebugging::UnregisterWidgetInputRoutingEvent(this);
	Super::Exit();
}

#undef LOCTEXT_NAMESPACE
