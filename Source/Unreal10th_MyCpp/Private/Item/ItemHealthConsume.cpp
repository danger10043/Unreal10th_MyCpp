
#include "Item/ItemHealthConsume.h"

#include "Component/StatComponent.h"
#include "Data/ItemDataAsset.h"
#include "Interface/HealthInterface.h"
#include "Interface/StatComponentInterface.h"

void AItemHealthConsume::InitializeFromItemData(UItemDataAsset* InItemData)
{
	Super::InitializeFromItemData(InItemData);

	if (!IsValid(InItemData))
	{
		return;
	}

	HealAmount = FMath::Max(0.0f, InItemData->StatAmount);
}

void AItemHealthConsume::PickUpItem(AActor* InActor)
{
	Super::PickUpItem(InActor);
}