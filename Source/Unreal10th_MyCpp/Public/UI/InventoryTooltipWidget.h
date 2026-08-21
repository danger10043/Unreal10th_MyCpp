// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTooltipWidget.generated.h"

class UImage;
class UTextBlock;
class UItemDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API UInventoryTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItemData(UItemDataAsset* ItemData);

	void HideTooltip();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemTooltip = nullptr;
};
