// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryCellWidget.generated.h"

class UImage;
class UTextBlock;
class UItemDataAsset;
class UDragDropOperation;
class UInventoryDragItemWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInventoryCellHovered,
	UItemDataAsset*,
	ItemData
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryCellUnhovered);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnInventoryCellItemDropped,
	int32,
	SourceSlotIndex,
	int32,
	TargetSlotIndex
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnInventoryCellDragCancelled,
	int32,
	SourceSlotIndex
);

UCLASS()
class UNREAL10TH_MYCPP_API UInventoryCellWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCellData(UItemDataAsset* ItemData, int32 Amount);

	void SetSelected(bool bSelected);

	void ClearCell();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryCellItemDropped OnCellItemDropped;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryCellDragCancelled OnCellDragCancelled;

	void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }

	int32 GetSlotIndex() const { return SlotIndex; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryCellHovered OnCellHovered;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryCellUnhovered OnCellUnhovered;


protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> SlotImage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> HighlightImage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Selection")
	TObjectPtr<UTexture2D> NormalSlotTexture = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Selection")
	TObjectPtr<UTexture2D> SelectedSlotTexture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UItemDataAsset> CurrentItemData = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Drag")
	TSubclassOf<UInventoryDragItemWidget> DragItemWidgetClass = nullptr;

	int32 CurrentAmount = 0;
	int32 SlotIndex = INDEX_NONE;
};
