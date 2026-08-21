// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDragItemWidget.generated.h"

class UImage;
class UTextBlock;
class UItemDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API UInventoryDragItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemData(UItemDataAsset* ItemData, int32 Amount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountText = nullptr;
};
