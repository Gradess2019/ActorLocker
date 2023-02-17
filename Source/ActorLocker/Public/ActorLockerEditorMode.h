// Copyright Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorLockerTypes.h"
#include "Tools/UEdMode.h"
#include "Tools/LegacyEdModeInterfaces.h"
#include "ActorLockerEditorMode.generated.h"

struct HActor;
class IAssetGenerationAPI;

/**
 * 
 */
UCLASS()
class ACTORLOCKER_API UActorLockerEditorMode : public UEdMode, public FSlateDebugging::IWidgetInputRoutingEvent, public ILegacyEdModeViewportInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	EActorLockerInteractionType InteractionType = EActorLockerInteractionType::None;

	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> OutlinerWidgetTypes;
	
	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> MenuWidgetTypes;
	
	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> LockerWidgetTypes;

	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> IgnoredWidgetTypes;

	TSet<uint32> SelectedItems;	

public:
	const static FEditorModeID EM_ActorLockerEditorModeId;

	UActorLockerEditorMode();

	virtual void Enter() override;
	virtual void Exit() override;
	virtual bool IsCompatibleWith(FEditorModeID OtherModeID) const override;
	virtual bool IsSelectionDisallowed(AActor* InActor, bool bInSelection) const override;

	virtual bool UsesToolkits() const override { return false; }

	void UnregisterEvent();
	void RegisterEvent();

	void UpdateWidgetTypes();

	//~ Begin FSlateDebugging::IWidgetInputRoutingEvent Interface
	virtual void OnProcessInput(ESlateDebuggingInputEvent InputEventType, const FInputEvent& Event) override;
	virtual void OnPreProcessInput(ESlateDebuggingInputEvent InputEventType, const TCHAR* InputPrecessorName, bool bHandled) override {}
	virtual void OnRouteInput(ESlateDebuggingInputEvent InputEventType, const FName& RoutedType) override {}
	virtual void OnInputEvent(ESlateDebuggingInputEvent InputEventType, const FReply& InReply, const TSharedPtr<SWidget>& HandlerWidget) override {}
	virtual void OnInputRouted(ESlateDebuggingInputEvent InputEventType) override {}
	virtual void OnInputProcessed(ESlateDebuggingInputEvent InputEventType) override {}
	//~ End FSlateDebugging::IWidgetInputRoutingEvent Interface

	//~ Begin ILegacyEdModeViewportInterface Interface
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	//~ End ILegacyEdModeViewportInterface Interface

protected:
	virtual FWidgetPath GetWidgetPath(const FInputEvent& Event) const;
	virtual EActorLockerInteractionType GetInteractionType(const FWidgetPath& Path, uint32& OutItemId) const;
	virtual void CheckLockedActorsSelection() const;

	bool IsAppropriateProxy(HHitProxy* HitProxy) const;
	bool SelectFirstUnlockedActor(const FEditorViewportClient* InViewportClient, const FViewportClick& Click) const;
	FVector GetTraceStart(const FVector& InClickOrigin , const FVector& InDirection, const float InOrthoHeight, const ELevelViewportType InViewportType) const;
	FVector GetTraceEnd(const FVector& InStart , const FVector& InDirection, const float InLength) const;
	
};
