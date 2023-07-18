// Copyright 2023 Gradess Games. All Rights Reserved.


#include "ActorLockerEditorMode.h"
#include "ActorLockerManager.h"
#include "ActorLockerSettings.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "SOutlinerTreeView.h"
#include "SSceneOutliner.h"
#include "Stats/StatsMisc.h"
#include "ActorLocker.h"
#include "EngineUtils.h"

#if OLDER_THAN_UE_5_1
#include "InstancedFoliageActor.h"
#endif


#define LOCTEXT_NAMESPACE "FSampleToolsEditorMode"

const FEditorModeID UActorLockerEditorMode::EM_ActorLockerEditorModeId = TEXT("EM_SampleToolsEditorMode");


UActorLockerEditorMode::UActorLockerEditorMode()
{
	Info = FEditorModeInfo(
		EM_ActorLockerEditorModeId,
		LOCTEXT("ActorLockerEditorModeName", "ActorLockerEditorMode"),
		FSlateIcon(),
		false, 0);

	UpdateWidgetTypes();
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
	const TSet<FName>& CompatibleModes = GetDefault<UActorLockerSettings>()->CompatibleModes;
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

#if OLDER_THAN_UE_5_1
	const auto bFoliageActor = InActor->GetLevel()->InstancedFoliageActor == InActor;
	const auto bDenied = bLocked || bLevelBrushActor || bFoliageActor;
#else
	const auto bDenied = bLocked || bLevelBrushActor;
#endif

	return bDenied;
}

void UActorLockerEditorMode::RegisterEvent()
{
	FSlateDebugging::RegisterWidgetInputRoutingEvent(this);
}

void UActorLockerEditorMode::UpdateWidgetTypes()
{
	const auto Settings = GetDefault<UActorLockerSettings>();
	OutlinerWidgetTypes = Settings->OutlinerWidgetTypes;
	MenuWidgetTypes = Settings->MenuWidgetTypes;
	LockerWidgetTypes = Settings->LockerWidgetTypes;
	IgnoredWidgetTypes = Settings->IgnoredWidgetTypes;
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
		InteractionType = GetInteractionType(WidgetPath, ItemId);

		switch (InteractionType)
		{
		case EActorLockerInteractionType::None:
			{
				CheckLockedActorsSelection();
				break;
			}
		case EActorLockerInteractionType::Outliner:
			{
				SelectedItems.Add(ItemId);
				break;
			}
		case EActorLockerInteractionType::Ignored: break;
		default: checkNoEntry();
		}
	}
}

bool UActorLockerEditorMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	if (Click.GetKey() != EKeys::LeftMouseButton)
	{
		return false;
	}

	if (!IsAppropriateProxy(HitProxy))
	{
		return ILegacyEdModeViewportInterface::HandleClick(InViewportClient, HitProxy, Click);
	}

	if (!Click.IsControlDown())
	{
		GEditor->GetSelectedActors()->DeselectAll();
	}

	return SelectFirstUnlockedActor(InViewportClient, Click);
}

FWidgetPath UActorLockerEditorMode::GetWidgetPath(const FInputEvent& Event) const
{
	const auto& PointerEvent = StaticCast<const FPointerEvent&>(Event);
	const auto Position = PointerEvent.GetScreenSpacePosition();

	auto& SlateApplication = FSlateApplication::Get();
	const auto Windows = SlateApplication.GetInteractiveTopLevelWindows();

	return FSlateApplication::Get().LocateWindowUnderMouse(Position, Windows);;
}

EActorLockerInteractionType UActorLockerEditorMode::GetInteractionType(const FWidgetPath& Path, uint32& OutItemId) const
{
	if (!Path.IsValid())
	{
		return EActorLockerInteractionType::None;
	}

	auto bResult = EActorLockerInteractionType::None;
	const auto& Widgets = Path.Widgets.GetInternalArray();
	TSharedPtr<SSceneOutlinerTreeRow> Row = nullptr;
	TSharedPtr<SSceneOutlinerTreeView> Tree = nullptr;

	for (const auto& ArrangedWidget : Widgets)
	{
		const auto Type = ArrangedWidget.Widget->GetType();

		if (InteractionType == EActorLockerInteractionType::Outliner && MenuWidgetTypes.Contains(Type))
		{
			bResult = EActorLockerInteractionType::Outliner;
			break;
		}

		if (LockerWidgetTypes.Contains(Type))
		{
			bResult = EActorLockerInteractionType::None;
			break;
		}

		if (IgnoredWidgetTypes.Contains(Type))
		{
			bResult = EActorLockerInteractionType::Ignored;
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
			bResult = EActorLockerInteractionType::Outliner;
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

bool UActorLockerEditorMode::IsAppropriateProxy(HHitProxy* HitProxy) const
{
	const auto ActorHitProxy = HitProxyCast<HActor>(HitProxy);
	if (!ActorHitProxy)
	{
		return false;
	}

	const auto LockerManager = UActorLockerManager::GetActorLockerManager();
	const auto ClickedActor = ActorHitProxy->Actor;
	return IsValid(ClickedActor) && LockerManager->IsActorLocked(ClickedActor);
}

bool UActorLockerEditorMode::SelectFirstUnlockedActor(const FEditorViewportClient* InViewportClient, const FViewportClick& Click) const
{
	const auto World = InViewportClient->GetWorld();
	const auto bOrtho = InViewportClient->IsOrtho();
	const auto Length = bOrtho ? InViewportClient->GetOrthoZoom() : HALF_WORLD_MAX1;
	const auto Direction = Click.GetDirection();

	const auto ClickOrigin = Click.GetOrigin();
	const auto ViewportType = InViewportClient->GetViewportType();
	const auto Start = GetTraceStart(ClickOrigin, Direction, Length, ViewportType);
	const auto End = GetTraceEnd(Start, Direction, Length);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.bTraceComplex = true;
	World->LineTraceMultiByObjectType(HitResults, Start, End, FCollisionObjectQueryParams::AllObjects, QueryParams);

	const auto LockerManager = UActorLockerManager::GetActorLockerManager();
	for (const auto& HitResult : HitResults)
	{
		const auto HitActor = HitResult.GetActor();
		if (IsValid(HitActor) && !LockerManager->IsActorLocked(HitActor))
		{
			GEditor->SelectActor(HitActor, true, true);
			return true;
		}
	}

	return false;
}

FVector UActorLockerEditorMode::GetTraceStart(const FVector& InClickOrigin, const FVector& InDirection, const float InOrthoHeight, const ELevelViewportType InViewportType) const
{
	auto NewClickOrigin = InClickOrigin;
	switch (InViewportType)
	{
	case LVT_OrthoXY:
	case LVT_OrthoXZ:
	case LVT_OrthoYZ:
	case LVT_OrthoNegativeXY:
	case LVT_OrthoNegativeXZ:
	case LVT_OrthoNegativeYZ:
		{
			NewClickOrigin.X = FMath::IsNearlyZero(InDirection.X) ? NewClickOrigin.X : -InOrthoHeight * InDirection.X;
			NewClickOrigin.Y = FMath::IsNearlyZero(InDirection.Y) ? NewClickOrigin.Y : -InOrthoHeight * InDirection.Y;
			NewClickOrigin.Z = FMath::IsNearlyZero(InDirection.Z) ? NewClickOrigin.Z : -InOrthoHeight * InDirection.Z;
			break;
		}
	case LVT_Perspective:
	case LVT_OrthoFreelook:
		{
			break;
		}
	default: checkNoEntry();
	}

	return NewClickOrigin;
}

FVector UActorLockerEditorMode::GetTraceEnd(const FVector& InStart, const FVector& InDirection, const float InLength) const
{
	const auto End = FVector(
		FMath::IsNearlyZero(InDirection.X) ? InStart.X : InDirection.X * InLength,
		FMath::IsNearlyZero(InDirection.Y) ? InStart.Y : InDirection.Y * InLength,
		FMath::IsNearlyZero(InDirection.Z) ? InStart.Z : InDirection.Z * InLength
	);

	return End;
}


#undef LOCTEXT_NAMESPACE
