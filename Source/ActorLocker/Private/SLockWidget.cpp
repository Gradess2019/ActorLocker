// Copyright 2023 Gradess Games. All Rights Reserved.


#include "SLockWidget.h"
#include "ActorLocker.h"
#include "ActorLockerManager.h"
#include "ActorLockerStyle.h"
#include "ActorTreeItem.h"
#include "ActorLockerTypes.h"
#include "Editor.h"
#include "ISceneOutliner.h"
#include "LevelTreeItem.h"
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

	auto& Module = FModuleManager::GetModuleChecked<FActorLockerModule>("ActorLocker");
	const auto bRequired = true;
	WeakActorManager = Module.GetActorLockerManager(bRequired);
	OnActorLockerManagerCreatedHandle = Module.OnActorLockerManagerCreated.AddLambda([this](UActorLockerManager* InNewManager) { WeakActorManager = InNewManager; });
	
	check(WeakActorManager.IsValid());
	WeakActorManager->InitItem(WeakItem);

	SImage::Construct(
		SImage::FArguments()
		.IsEnabled(this, &SLockWidget::IsEnabled)
		.ColorAndOpacity(this, &SLockWidget::GetForegroundColor)
		.Image(this, &SLockWidget::GetBrush)
	);
}

SLockWidget::~SLockWidget()
{
	if (FModuleManager::Get().IsModuleLoaded("ActorLocker"))
	{
		const auto Module = FModuleManager::GetModulePtr<FActorLockerModule>("ActorLocker");
		Module->OnActorLockerManagerCreated.Remove(OnActorLockerManagerCreatedHandle);
	}
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
		return FActorLockerStyle::Get().GetBrush(TEXT("SceneOutliner.Lock"));
	}
	return FActorLockerStyle::Get().GetBrush(TEXT("SceneOutliner.Unlock"));
}

bool SLockWidget::IsLocked() const
{
	return IsLocked(WeakItem.Pin(), WeakColumn.Pin());
}

bool SLockWidget::IsLocked(const TWeakPtr<ISceneOutlinerTreeItem>& Item, const TSharedPtr<FSceneOutlinerActorLocker>& Column) const
{
	return Item.IsValid() && Column.IsValid() ? WeakActorManager.Get()->IsItemLocked(Item) : false;
}

void SLockWidget::SetIsLocked(const bool bNewLocked)
{
	const auto bLocked = IsLocked();
	if (WeakItem.IsValid() && WeakOutliner.IsValid() && WeakActorManager.IsValid() && bLocked != bNewLocked)
	{
		WeakActorManager.Get()->SetLockTreeItem(WeakItem, bNewLocked);
		WeakOutliner.Pin()->Refresh();
		GEditor->RedrawAllViewports();
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
	
	const auto& Tree = Outliner->GetTree();
	const bool bNewLocked = !IsLocked();

	const auto TransactionName = bNewLocked ? TEXT("Lock object") : TEXT("Unlock object");
	GEditor->BeginTransaction(FText::FromString(TransactionName));
	SaveToTransactionBuffer(WeakActorManager.Get(), false);

	// We operate on all the selected items if the specified item is selected
	if (Tree.IsItemSelected(TreeItem.ToSharedRef()))
	{
		for (auto& SelectedItem : Tree.GetSelectedItems())
		{
			if (IsLocked(SelectedItem, Column) != bNewLocked)
			{
				WeakActorManager.Get()->SetLockTreeItem(SelectedItem, bNewLocked);
			}
		}

		GEditor->RedrawAllViewports();
	}
	else
	{
		SetIsLocked(bNewLocked);
	}

	GEditor->EndTransaction();

	return FReply::Handled();
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
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
