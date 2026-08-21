// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUIWidget.generated.h"

class UHorizontalBox;
class UInventoryCellWidget;
class UInventoryComponent;
class UWidgetAnimation;

UCLASS()
class UNREAL10TH_MYCPP_API UInventoryUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsInventoryOpened() const { return bIsInventoryOpened; }

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> InventoryCells = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopUpInventory = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopDownInventory = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UInventoryCellWidget>> Cells;

	bool bIsInventoryOpened = false;
};
