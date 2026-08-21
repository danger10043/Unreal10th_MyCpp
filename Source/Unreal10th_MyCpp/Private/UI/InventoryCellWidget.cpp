// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryCellWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/ItemDataAsset.h"
#include "Engine/Texture2D.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "UI/InventoryDragItemWidget.h"

void UInventoryCellWidget::SetSelected(bool bSelected)
{
	if (!IsValid(SlotImage)) return;

	UTexture2D* TargetTexture = bSelected ? SelectedSlotTexture : NormalSlotTexture;

	if (IsValid(TargetTexture))
	{
		SlotImage->SetBrushFromTexture(TargetTexture);
	}
}

void UInventoryCellWidget::SetCellData(UItemDataAsset* ItemData, int32 Amount)
{
	if (!IsValid(ItemData) || Amount <= 0)
	{
		ClearCell();
		return;
	}

	CurrentItemData = ItemData;
	CurrentAmount = Amount;

	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(ItemIcon))
	{
		UTexture2D* LoadedIcon = ItemData->Icon.LoadSynchronous();

		ItemIcon->SetBrushFromTexture(LoadedIcon);
		ItemIcon->SetVisibility(IsValid(LoadedIcon) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (IsValid(AmountText))
	{
		AmountText->SetText(FText::AsNumber(Amount));
		AmountText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventoryCellWidget::ClearCell()
{
	CurrentItemData = nullptr;
	CurrentAmount = 0;
	OnCellUnhovered.Broadcast();

	if (IsValid(ItemIcon))
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(AmountText))
	{
		AmountText->SetText(FText::GetEmpty());
		AmountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryCellWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetSelected(false);

	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryCellWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (IsValid(CurrentItemData))
	{
		OnCellHovered.Broadcast(CurrentItemData);
	}
}

void UInventoryCellWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}
	OnCellUnhovered.Broadcast();
}

FReply UInventoryCellWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton
		&& IsValid(CurrentItemData)
		&& CurrentAmount > 0)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(
			InMouseEvent,
			this,
			EKeys::LeftMouseButton
		).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryCellWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!IsValid(CurrentItemData) || CurrentAmount <= 0 || !DragItemWidgetClass) return;

	UInventoryDragItemWidget* DragVisual = CreateWidget<UInventoryDragItemWidget>(
		GetOwningPlayer(),
		DragItemWidgetClass
	);

	if (!IsValid(DragVisual)) return;

	DragVisual->SetItemData(CurrentItemData, CurrentAmount);

	UDragDropOperation* DragOperation = NewObject<UDragDropOperation>(this);

	DragOperation->Payload = this;
	DragOperation->DefaultDragVisual = DragVisual;
	DragOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragOperation;
}

void UInventoryCellWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	if (!IsValid(InOperation)) return;

	UInventoryCellWidget* SourceCell = Cast<UInventoryCellWidget>(InOperation->Payload);

	if (!IsValid(SourceCell) || SourceCell == this) return;

	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UInventoryCellWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UInventoryCellWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (!IsValid(InOperation)) return false;

	UInventoryCellWidget* SourceCell = Cast<UInventoryCellWidget>(InOperation->Payload);

	if (!IsValid(SourceCell)) return false;

	const int32 SourceIndex = SourceCell->GetSlotIndex();

	if (SourceIndex == INDEX_NONE || SlotIndex == INDEX_NONE) return false;

	OnCellItemDropped.Broadcast(SourceIndex, SlotIndex);

	return true;
}

void UInventoryCellWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	if (IsValid(HighlightImage))
	{
		HighlightImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (SlotIndex != INDEX_NONE && IsValid(CurrentItemData) && CurrentAmount > 0)
	{
		OnCellDragCancelled.Broadcast(SlotIndex);
	}

	OnCellUnhovered.Broadcast();
}
