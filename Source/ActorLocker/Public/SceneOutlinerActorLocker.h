// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneOutlinerGutter.h"
#include "ISceneOutlinerColumn.h"

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 1
#include "SceneOutlinerPublicTypes.h"
#endif

#define LOCTEXT_NAMESPACE "SceneOutlinerLock"

/**
 * The Scene Outliner column that displays the lock state of actors
 */
class ACTORLOCKER_API FSceneOutlinerActorLocker : public ISceneOutlinerColumn
{
protected:
	TWeakPtr<ISceneOutliner> WeakOutliner;
	
public:
	FSceneOutlinerActorLocker(ISceneOutliner& Outliner);

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 1
	DEFINE_SCENEOUTLINER_BUILTIN_COLUMN_TYPE(Lock, "Lock", "LockColumnName", "Lock");
#else
	static FName& Lock()
	{
		static FName Lock = "Lock";
		return Lock;
	}

	static const FText& Lock_Localized()
	{
		static FText LockLocalized = LOCTEXT("LockColumnName", "Lock");
		return LockLocalized;
	}
#endif

	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;
	virtual const TSharedRef<SWidget> ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;

	static FName GetID() { return Lock(); }
	virtual FName GetColumnID() override { return GetID(); }
};

#undef LOCTEXT_NAMESPACE