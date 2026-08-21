// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryTooltipWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/ItemDataAsset.h"
#include "Engine/Texture2D.h"

void UInventoryTooltipWidget::SetItemData(UItemDataAsset* ItemData)
{
	if (!IsValid(ItemData))
	{
		HideTooltip();
		return;
	}

	if (IsValid(ItemIcon))
	{
		UTexture2D* LoadedIcon = ItemData->Icon.LoadSynchronous();

		ItemIcon->SetBrushFromTexture(LoadedIcon);
		ItemIcon->SetVisibility(IsValid(LoadedIcon) ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (IsValid(ItemName))
	{
		ItemName->SetText(ItemData->DisplayName);
	}

	if (IsValid(ItemTooltip))
	{
		ItemTooltip->SetText(ItemData->Description);
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInventoryTooltipWidget::HideTooltip()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HideTooltip();
}
