// Copyright 2023 Gradess Games. All Rights Reserved.

#include "SceneOutlinerActorLocker.h"
#include "ActorLockerStyle.h"
#include "ActorTreeItem.h"
#include "ISceneOutliner.h"
#include "ISceneOutlinerTreeItem.h"
#include "SLockWidget.h"

FSceneOutlinerActorLocker::FSceneOutlinerActorLocker(ISceneOutliner& Outliner)
{
	WeakOutliner = StaticCastSharedRef<ISceneOutliner>(Outliner.AsShared());
}

SHeaderRow::FColumn::FArguments FSceneOutlinerActorLocker::ConstructHeaderRowColumn()
{
	return SHeaderRow::Column(GetColumnID())
	       .FixedWidth(24.f)
	       .HAlignHeader(HAlign_Center)
	       .VAlignHeader(VAlign_Center)
	       .HAlignCell(HAlign_Center)
	       .VAlignCell(VAlign_Center)
	       .DefaultTooltip(FText::FromName(GetColumnID()))
	       .HeaderContentPadding(FMargin(0.f))
			[
				SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FActorLockerStyle::Get().GetBrush("SceneOutliner.Lock"))
			];
}

const TSharedRef<SWidget> FSceneOutlinerActorLocker::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	if (TreeItem->ShouldShowVisibilityState())
	{
		return SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(2.f, 0.f, 0.f, 0.f)
			[
				SNew(SLockWidget, SharedThis(this), WeakOutliner, TreeItem, &Row)
			];
	}
	return SNullWidget::NullWidget;
}