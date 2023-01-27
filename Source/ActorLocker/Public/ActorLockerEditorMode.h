// Copyright Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "ActorLockerEditorMode.generated.h"

class IAssetGenerationAPI;

/**
 * 
 */
UCLASS()
class ACTORLOCKER_API UActorLockerEditorMode : public UEdMode, public FSlateDebugging::IWidgetInputRoutingEvent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	bool bOutlinerInteraction = false;

	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> OutlinerWidgetTypes;
	
	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> MenuWidgetTypes;
	
	UPROPERTY(BlueprintReadOnly, Category = "Actor Locker Editor Mode")
	TSet<FName> LockerWidgetTypes;

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

	//~ Begin FSlateDebugging::IWidgetInputRoutingEvent Interface
	virtual void OnProcessInput(ESlateDebuggingInputEvent InputEventType, const FInputEvent& Event) override;
	virtual void OnPreProcessInput(ESlateDebuggingInputEvent InputEventType, const TCHAR* InputPrecessorName, bool bHandled) override {}
	virtual void OnRouteInput(ESlateDebuggingInputEvent InputEventType, const FName& RoutedType) override {}
	virtual void OnInputEvent(ESlateDebuggingInputEvent InputEventType, const FReply& InReply, const TSharedPtr<SWidget>& HandlerWidget) override {}
	virtual void OnInputRouted(ESlateDebuggingInputEvent InputEventType) override {}
	virtual void OnInputProcessed(ESlateDebuggingInputEvent InputEventType) override {}
	//~ End FSlateDebugging::IWidgetInputRoutingEvent Interface

protected:
	virtual FWidgetPath GetWidgetPath(const FInputEvent& Event) const;
	virtual bool IsOutlinerInteraction(const FWidgetPath& Path, uint32& OutItemId) const;
	virtual void CheckLockedActorsSelection() const;
	
};
