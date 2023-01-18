// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneOutlinerGutter.h"
#include "SceneOutlinerPublicTypes.h"
#include "ISceneOutlinerColumn.h"

#define LOCTEXT_NAMESPACE "SceneOutlinerLock"

/**
 * The Scene Outliner column that displays the lock state of actors
 */
class ACTORLOCKER_API FSceneOutlinerActorLocker : public ISceneOutlinerColumn
{
protected:
	TWeakPtr<ISceneOutliner> WeakOutliner;
	
public:
	DEFINE_SCENEOUTLINER_BUILTIN_COLUMN_TYPE(Lock, "Lock", "LockColumnName", "Lock");

	FSceneOutlinerActorLocker(ISceneOutliner& Outliner);

	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef<SWidget> ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;

	static FName GetID() { return Lock(); }
	virtual FName GetColumnID() override { return GetID(); }
};

#undef LOCTEXT_NAMESPACE