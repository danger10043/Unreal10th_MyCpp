// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryDragItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/ItemDataAsset.h"
#include "Engine/Texture2D.h"

void UInventoryDragItemWidget::SetItemData(UItemDataAsset* ItemData, int32 Amount)
{
	if (!IsValid(ItemData) || Amount <= 0) return;

	if (IsValid(ItemIcon))
	{
		UTexture2D* LoadedIcon = ItemData->Icon.LoadSynchronous();

		if (IsValid(LoadedIcon))
		{
			ItemIcon->SetBrushFromTexture(LoadedIcon);
			ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

	if (IsValid(AmountText))
	{
		AmountText->SetText(FText::AsNumber(Amount));
		AmountText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
