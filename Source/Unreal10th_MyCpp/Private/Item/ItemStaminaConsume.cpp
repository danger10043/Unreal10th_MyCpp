
#include "Item/ItemStaminaConsume.h"

#include "Component/StatComponent.h"
#include "Data/ItemDataAsset.h"
#include "Interface/StaminaInterface.h"
#include "Interface/StatComponentInterface.h"

void AItemStaminaConsume::InitializeFromItemData(
	UItemDataAsset* InItemData)
{
	Super::InitializeFromItemData(InItemData);

	if (!IsValid(InItemData))
	{
		return;
	}

	StaminaAmount = FMath::Max(
		0.0f,
		InItemData->StatAmount
	);
}

void AItemStaminaConsume::PickUpItem(AActor* InActor)
{
	Super::PickUpItem(InActor);
}