#pragma once

#include "CoreMinimal.h"
#include "UI/StatProgressUI.h"
#include "MainUIWidget.generated.h"

class UHorizontalBox;
class UHealthBarWidget;
class UStaminaBarWidget;
class UInventoryCellWidget;
class UInventoryTooltipWidget;
class UInventoryComponent;
class UWidgetAnimation;
class UItemDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API UMainUIWidget : public UStatProgressUI
{
	GENERATED_BODY()

public:
	virtual void SetHealth(float CurrentValue, float MaxValue) override;

	virtual void SetStamina(float CurrentValue, float MaxValue) override;

	void PlayTakeDamageAnimation();

	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsInventoryOpened() const { return bIsInventoryOpened; }

	void SelectInventorySlot(int32 SlotIndex);

	int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void ShowInventoryTooltip(UItemDataAsset* ItemData);

	UFUNCTION()
	void HideInventoryTooltip();

	UFUNCTION()
	void HandleInventoryCellDrop(int32 SourceSlotIndex, int32 TargetSlotIndex);

	UFUNCTION()
	void HandleInventoryCellDragCancelled(int32 SourceSlotIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> HealthBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UStaminaBarWidget> StaminaBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> InventoryCells = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInventoryTooltipWidget> Tooltip = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopUpInventory = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopDownInventory = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UInventoryCellWidget>> Cells;

	bool bIsInventoryOpened = false;

	int32 SelectedSlotIndex = INDEX_NONE;
};