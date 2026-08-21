#include "UI/MainUIWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Component/InventoryComponent.h"
#include "Components/HorizontalBox.h"
#include "Data/ItemDataAsset.h"
#include "GameFramework/PlayerController.h"
#include "UI/InventoryCellWidget.h"
#include "UI/InventoryTooltipWidget.h"
#include "UI/HealthBarWidget.h"
#include "UI/StaminaBarWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InputCoreTypes.h"
#include "UI/InventoryDragItemWidget.h"
#include "Player/ActionPlayer.h"
#include "Framework/ItemFactorySubsystem.h"
#include "Item/ItemBase.h"


void UMainUIWidget::SetHealth(float CurrentValue, float MaxValue)
{
	if (IsValid(HealthBar))
	{
		HealthBar->SetHealth(CurrentValue, MaxValue);
	}
}

void UMainUIWidget::SetStamina(float CurrentValue, float MaxValue)
{
	if (IsValid(StaminaBar))
	{
		StaminaBar->SetStamina(CurrentValue, MaxValue);
	}
}

void UMainUIWidget::PlayTakeDamageAnimation()
{
	if (IsValid(HealthBar))
	{
		HealthBar->PlayTakeDamageAnimation();
	}
}

void UMainUIWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	if (InventoryComponent == InInventoryComponent)
	{
		return;
	}

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UMainUIWidget::RefreshInventory
		);
	}

	InventoryComponent = InInventoryComponent;

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(
			this,
			&UMainUIWidget::RefreshInventory
		);
	}

	RefreshInventory();
}

void UMainUIWidget::OpenInventory()
{
	if (bIsInventoryOpened)
	{
		return;
	}

	bIsInventoryOpened = true;
	SelectedSlotIndex = INDEX_NONE;

	for (UInventoryCellWidget* Cell : Cells)
	{
		if (IsValid(Cell))
		{
			Cell->SetSelected(false);
		}
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
	}

	if (IsValid(PopDownInventory))
	{
		StopAnimation(PopDownInventory);
	}

	if (IsValid(PopUpInventory))
	{
		PlayAnimation(PopUpInventory);
	}
}

void UMainUIWidget::CloseInventory()
{
	if (!bIsInventoryOpened)
	{
		return;
	}

	bIsInventoryOpened = false;

	HideInventoryTooltip();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (IsValid(PopUpInventory))
	{
		StopAnimation(PopUpInventory);
	}

	if (IsValid(PopDownInventory))
	{
		PlayAnimation(PopDownInventory);
	}
}

void UMainUIWidget::ToggleInventory()
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

void UMainUIWidget::SelectInventorySlot(int32 SlotIndex)
{
	if (!bIsInventoryOpened || !Cells.IsValidIndex(SlotIndex)) return;

	if (SelectedSlotIndex == SlotIndex) return;

	if (Cells.IsValidIndex(SelectedSlotIndex) && IsValid(Cells[SelectedSlotIndex]))
	{
		Cells[SelectedSlotIndex]->SetSelected(false);
	}

	SelectedSlotIndex = SlotIndex;

	if (IsValid(Cells[SelectedSlotIndex]))
	{
		Cells[SelectedSlotIndex]->SetSelected(true);
	}
}

void UMainUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Tooltip))
	{
		Tooltip->HideTooltip();
	}

	Cells.Empty();

	if (IsValid(InventoryCells))
	{
		const int32 ChildCount = InventoryCells->GetChildrenCount();

		for (int32 Index = 0; Index < ChildCount; ++Index)
		{
			UInventoryCellWidget* Cell =
				Cast<UInventoryCellWidget>(InventoryCells->GetChildAt(Index));

			if (IsValid(Cell))
			{
				Cells.Add(Cell);
				Cell->SetSlotIndex(Cells.Num() - 1);

				Cell->OnCellItemDropped.RemoveDynamic(
					this,
					&UMainUIWidget::HandleInventoryCellDrop
				);

				Cell->OnCellItemDropped.AddDynamic(
					this,
					&UMainUIWidget::HandleInventoryCellDrop
				);

				Cell->OnCellDragCancelled.RemoveDynamic(
					this,
					&UMainUIWidget::HandleInventoryCellDragCancelled
				);

				Cell->OnCellDragCancelled.AddDynamic(
					this,
					&UMainUIWidget::HandleInventoryCellDragCancelled
				);

				Cell->OnCellHovered.RemoveDynamic(
					this,
					&UMainUIWidget::ShowInventoryTooltip
				);

				Cell->OnCellUnhovered.RemoveDynamic(
					this,
					&UMainUIWidget::HideInventoryTooltip
				);

				Cell->OnCellHovered.AddDynamic(
					this,
					&UMainUIWidget::ShowInventoryTooltip
				);

				Cell->OnCellUnhovered.AddDynamic(
					this,
					&UMainUIWidget::HideInventoryTooltip
				);
			}
		}
	}

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UMainUIWidget::RefreshInventory
		);

		InventoryComponent->OnInventoryChanged.AddDynamic(
			this,
			&UMainUIWidget::RefreshInventory
		);

		RefreshInventory();
	}

	bIsInventoryOpened = false;

	if (IsValid(PopDownInventory))
	{
		PlayAnimation(PopDownInventory);
	}
}

void UMainUIWidget::NativeDestruct()
{
	for (UInventoryCellWidget* Cell : Cells)
	{
		if (!IsValid(Cell)) continue;

		Cell->OnCellItemDropped.RemoveDynamic(
			this,
			&UMainUIWidget::HandleInventoryCellDrop
		);

		Cell->OnCellDragCancelled.RemoveDynamic(
			this,
			&UMainUIWidget::HandleInventoryCellDragCancelled
		);

		Cell->OnCellHovered.RemoveDynamic(
			this,
			&UMainUIWidget::ShowInventoryTooltip
		);

		Cell->OnCellUnhovered.RemoveDynamic(
			this,
			&UMainUIWidget::HideInventoryTooltip
		);
	}

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(
			this,
			&UMainUIWidget::RefreshInventory
		);
	}

	Super::NativeDestruct();
}

void UMainUIWidget::RefreshInventory()
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

	const TArray<FInventoryData> InventoryItems =
		InventoryComponent->GetInventoryItems();

	const int32 DisplayCount =
		FMath::Min(InventoryItems.Num(), Cells.Num());

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

void UMainUIWidget::ShowInventoryTooltip(UItemDataAsset* ItemData)
{
	if (!bIsInventoryOpened || !IsValid(Tooltip)) return;

	Tooltip->SetItemData(ItemData);
}

void UMainUIWidget::HideInventoryTooltip()
{
	if (IsValid(Tooltip))
	{
		Tooltip->HideTooltip();
	}
}

void UMainUIWidget::HandleInventoryCellDrop(int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	if (!bIsInventoryOpened || !IsValid(InventoryComponent)) return;

	InventoryComponent->SwapItemsAtIndices(SourceSlotIndex, TargetSlotIndex);
}

void UMainUIWidget::HandleInventoryCellDragCancelled(int32 SourceSlotIndex)
{
	const AActionPlayer* TargetPlayer = Cast<AActionPlayer>(GetOwningPlayerPawn());

	if (!IsValid(TargetPlayer) || !bIsInventoryOpened || !IsValid(InventoryComponent)) return;

	const TArray<FInventoryData>& InventoryItems = InventoryComponent->GetItems();

	if (!InventoryItems.IsValidIndex(SourceSlotIndex)) return;

	const FInventoryData InventoryData = InventoryItems[SourceSlotIndex];

	if (!IsValid(InventoryData.Item) || InventoryData.Amount <= 0) return;

	UWorld* World = GetWorld();

	if (!IsValid(World)) return;

	UItemFactorySubsystem* ItemFactory = World->GetSubsystem<UItemFactorySubsystem>();
	if (!IsValid(ItemFactory))
	{
		return;
	}

	const FVector SpawnLocation =
		TargetPlayer->GetActorLocation()
		+ TargetPlayer->GetActorForwardVector() * 100.0f
		+ FVector::UpVector * 100.0f;

	const FTransform SpawnTransform(TargetPlayer->GetActorRotation(), SpawnLocation);

	TArray<AItemBase*> SpawnedItems;
	SpawnedItems.Reserve(InventoryData.Amount);

	for (int32 Index = 0; Index < InventoryData.Amount; ++Index)
	{
		AItemBase* SpawnedItem = ItemFactory->SpawnItemAt(InventoryData.Item, SpawnTransform, true, TargetPlayer->GetActorForwardVector());

		if (!IsValid(SpawnedItem)) break;

		SpawnedItems.Add(SpawnedItem);
	}

	if (SpawnedItems.IsEmpty()) return;

	if (!InventoryComponent->RemoveItemAtIndex(SourceSlotIndex, SpawnedItems.Num()))
	{
		for (AItemBase* SpawnedItem : SpawnedItems)
		{
			if (IsValid(SpawnedItem))
			{
				SpawnedItem->Destroy();
			}
		}
	}
}
