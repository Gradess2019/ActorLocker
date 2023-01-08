// Copyright Gradess Games. All Rights Reserved.


#include "SLockWidget.h"

#include "ActorLockerStyle.h"
#include "ActorTreeItem.h"
#include "Editor.h"
#include "ISceneOutliner.h"
#include "SceneOutlinerActorLocker.h"
#include "SlateOptMacros.h"
#include "Widgets/Views/STreeView.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLockWidget::Construct(const FArguments& InArgs, TWeakPtr<FSceneOutlinerActorLocker> InWeakColumn, TWeakPtr<ISceneOutliner> InWeakOutliner, TWeakPtr<ISceneOutlinerTreeItem> InWeakItem, const STableRow<FSceneOutlinerTreeItemPtr>* InRow)
{
	WeakColumn = InWeakColumn;
	WeakOutliner = InWeakOutliner;
	WeakItem = InWeakItem;
	Row = InRow;

	SImage::Construct(
		SImage::FArguments()
		.IsEnabled(this, &SLockWidget::IsEnabled)
		.ColorAndOpacity(this, &SLockWidget::GetForegroundColor)
		.Image(this, &SLockWidget::GetBrush)
	);
}

FSlateColor SLockWidget::GetForegroundColor() const
{
	const auto Outliner = WeakOutliner.Pin();
	const auto TreeItem = WeakItem.Pin();

	const bool bIsSelected = Outliner->GetTree().IsItemSelected(TreeItem.ToSharedRef());

	if (!IsLocked() && !Row->IsHovered() && !bIsSelected)
	{
		return FLinearColor::Transparent;
	}
	else if (IsHovered() && !bIsSelected)
	{
		return FAppStyle::Get().GetSlateColor("Colors.ForegroundHover");
	}

	return FSlateColor::UseForeground();
}

const FSlateBrush* SLockWidget::GetBrush() const
{
	if (IsLocked())
	{
		return FActorLockerStyle::Get().GetBrush(TEXT("SceneOutliner.LockClosed"));
	}
	return FActorLockerStyle::Get().GetBrush(TEXT("SceneOutliner.LockOpened"));
}

bool SLockWidget::IsLocked() const
{
	return IsLocked(WeakItem.Pin(), WeakColumn.Pin());
}

bool SLockWidget::IsLocked(const TWeakPtr<ISceneOutlinerTreeItem>& Item, const TSharedPtr<FSceneOutlinerActorLocker>& Column) const
{
	return Item.IsValid() && Column.IsValid() ? Column.Get()->IsItemLocked(Item) : false;
}

void SLockWidget::SetIsLocked(const bool bNewLocked)
{
	const auto bLocked = IsLocked();
	if (WeakItem.IsValid() && WeakOutliner.IsValid() && bLocked != bNewLocked)
	{
		OnSetItemLocked(WeakItem, bNewLocked);
			
		WeakOutliner.Pin()->Refresh();

		GEditor->RedrawAllViewports();
	}
}

void SLockWidget::OnSetItemLocked(const TWeakPtr<ISceneOutlinerTreeItem>& Item, const bool bNewLocked)
{
	WeakColumn.Pin()->SetItemLocked(Item, bNewLocked);

	TWeakPtr<FActorTreeItem> ActorItem = StaticCastWeakPtr<FActorTreeItem, ISceneOutlinerTreeItem>(Item);
	
	// TODO: Implement locking here
	
	for (auto& ChildPtr : Item.Pin()->GetChildren())
	{
		auto Child = ChildPtr.Pin();
		if (Child.IsValid())
		{
			OnSetItemLocked(Child, bNewLocked);
		}
	}
}

FReply SLockWidget::HandleClick()
{
	if (!IsEnabled())
	{
		return FReply::Unhandled();
	}

	const auto Outliner = WeakOutliner.Pin();
	const auto TreeItem = WeakItem.Pin();
	const auto Column = WeakColumn.Pin();
		
	if (!Outliner.IsValid() || !TreeItem.IsValid() || !Column.IsValid())
	{
		return FReply::Unhandled();
	}

	UndoTransaction.Reset(new FScopedTransaction(NSLOCTEXT("SceneOutlineActorLock", "SetOutlinerItemLock", "Set Item Lock")));

	const auto& Tree = Outliner->GetTree();
	const bool bNewLocked = !IsLocked();

	// We operate on all the selected items if the specified item is selected
	if (Tree.IsItemSelected(TreeItem.ToSharedRef()))
	{
		for (auto& SelectedItem : Tree.GetSelectedItems())
		{
			if (IsLocked(SelectedItem, Column) != bNewLocked)
			{
				OnSetItemLocked(SelectedItem, bNewLocked);
			}		
		}

		GEditor->RedrawAllViewports();
	}
	else
	{
		SetIsLocked(bNewLocked);
	}

	return FReply::Unhandled();
}

FReply SLockWidget::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return HandleClick();
}

FReply SLockWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return FReply::Unhandled();
	}

	return HandleClick();
}

FReply SLockWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UndoTransaction.Reset();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SLockWidget::OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	UndoTransaction.Reset();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
