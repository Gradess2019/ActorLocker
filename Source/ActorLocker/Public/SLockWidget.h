// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 0
#include "SceneOutlinerFwd.h"
#else
#include "SceneOutliner/Public/SceneOutlinerFwd.h"
#endif

#include "Widgets/Images/SImage.h"
#include "Widgets/Views/STableRow.h"
#include "ActorLockerTypes.h"

class UActorLockerManager;
class ISceneOutliner;
class FSceneOutlinerActorLocker;
struct ISceneOutlinerTreeItem;

/**
 * The widget that represent Lock button in Scene Outliner
 */
class ACTORLOCKER_API SLockWidget : public SImage
{
public:
	SLATE_BEGIN_ARGS(SLockWidget) {}
	SLATE_END_ARGS()

protected:
	FDelegateHandle OnActorLockerManagerCreatedHandle;
	
	TWeakPtr<FSceneOutlinerActorLocker> WeakColumn;
	TWeakPtr<ISceneOutliner> WeakOutliner;
	TWeakPtr<ISceneOutlinerTreeItem> WeakItem;
	TWeakObjectPtr<UActorLockerManager> WeakActorManager;
	
	const STableRow<FSceneOutlinerTreeItemPtr>* Row = nullptr;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakPtr<FSceneOutlinerActorLocker> InWeakColumn, TWeakPtr<ISceneOutliner> InWeakOutliner, TWeakPtr<ISceneOutlinerTreeItem> InWeakTreeItem, const STableRow<FSceneOutlinerTreeItemPtr>* InRow);

	~SLockWidget();
protected:
	virtual FSlateColor GetForegroundColor() const override;
	virtual const FSlateBrush* GetBrush() const;
	
	virtual bool IsEnabled() const { return true; }
	virtual bool IsLocked() const;
	virtual bool IsLocked(const TWeakPtr<ISceneOutlinerTreeItem>& Item, const TSharedPtr<FSceneOutlinerActorLocker>& Column) const;
	virtual void SetIsLocked(const bool bNewLocked);

	FReply HandleClick();
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};
