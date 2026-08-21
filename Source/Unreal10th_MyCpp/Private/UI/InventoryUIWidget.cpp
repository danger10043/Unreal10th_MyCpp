// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryUIWidget.h"

#include "Component/InventoryComponent.h"
#include "Components/HorizontalBox.h"
#include "Data/ItemDataAsset.h"
#include "UI/InventoryCellWidget.h"
#include "Animation/WidgetAnimation.h"

void UInventoryUIWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	if (InventoryComponent == InInventoryComponent)
	{
		return;
	}
	
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UInventoryUIWidget::RefreshInventory
		);
	}

	InventoryComponent = InInventoryComponent;

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(
			this,
			&UInventoryUIWidget::RefreshInventory
		);
	}

	RefreshInventory();
}

void UInventoryUIWidget::OpenInventory()
{
	if (bIsInventoryOpened) return;

	bIsInventoryOpened = true;

	if (IsValid(PopDownInventory))
	{
		StopAnimation(PopDownInventory);
	}

	if (IsValid(PopUpInventory))
	{
		PlayAnimation(PopUpInventory);
	}
}

void UInventoryUIWidget::CloseInventory()
{
	if (!bIsInventoryOpened) return;

	bIsInventoryOpened = false;

	if (IsValid(PopUpInventory))
	{
		StopAnimation(PopUpInventory);
	}

	if (IsValid(PopDownInventory))
	{
		PlayAnimation(PopDownInventory);
	}
}

void UInventoryUIWidget::ToggleInventory()
{
	if (bIsInventoryOpened)
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
	}
}

void UInventoryUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Cells.Empty();

	if (IsValid(InventoryCells))
	{
		const int32 ChildCount = InventoryCells->GetChildrenCount();

		for (int32 Index = 0; Index < ChildCount; ++Index)
		{
			UInventoryCellWidget* Cell = Cast<UInventoryCellWidget>(InventoryCells->GetChildAt(Index));

			if (IsValid(Cell))
			{
				Cells.Add(Cell);
			}
		}
	}

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UInventoryUIWidget::RefreshInventory
		);

		InventoryComponent->OnInventoryChanged.AddDynamic(
			this,
			&UInventoryUIWidget::RefreshInventory
		);

		RefreshInventory();

		bIsInventoryOpened = false;
	}

	if (IsValid(PopDownInventory))
	{
		PlayAnimation(PopDownInventory);
	}
}

void UInventoryUIWidget::NativeDestruct()
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UInventoryUIWidget::RefreshInventory
		);
	}

	Super::NativeDestruct();
}

void UInventoryUIWidget::RefreshInventory()
{
	for (UInventoryCellWidget* Cell : Cells)
	{
		if (IsValid(Cell))
		{
			Cell->ClearCell();
		}
	}

	if (!IsValid(InventoryComponent))
	{
		return;
	}

	const TArray<FInventoryData> InventoryItems = InventoryComponent->GetInventoryItems();
	const int32 DisplayCount = FMath::Min(InventoryItems.Num(), Cells.Num());

	for (int32 Index = 0; Index < DisplayCount; ++Index)
	{
		if (!IsValid(Cells[Index]))
		{
			continue;
		}

		Cells[Index]->SetCellData(
			InventoryItems[Index].Item,
			InventoryItems[Index].Amount
		);
	}
}
