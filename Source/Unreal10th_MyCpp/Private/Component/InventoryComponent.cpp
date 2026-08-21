
#include "Component/InventoryComponent.h"
#include "Data/ItemDataAsset.h"
#include "Item/ItemUseEffect.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

bool UInventoryComponent::AddItem(UItemDataAsset* ItemData, int32 Amount)
{
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - ItemData가 유효하지 않습니다."));
		return false;
	}
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - 1보다 작은 개수의 아이템을 추가할 수 없습니다."));
		return false;
	}

	const int32 StackLimit = ItemData->bStackable ? FMath::Max(1, ItemData->MaxStackAmount) : 1;
	int32 AvailableAmount = 0;

	for (const FInventoryData& InventoryData : Items)
	{
		if (InventoryData.Item == ItemData)
		{
			AvailableAmount += FMath::Max(0, StackLimit - InventoryData.Amount);
		}
		else if (!IsValid(InventoryData.Item))
		{
			AvailableAmount += StackLimit;
		}
	}
	

	if (AvailableAmount < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - 인벤토리에 공간이 부족합니다. 요청=%d, 저장 가능=%d"), Amount, AvailableAmount);
		return false;
	}
	
	int32 RemainingAmount = Amount;

	for (FInventoryData& InventoryData : Items)
	{
		if (InventoryData.Item != ItemData || InventoryData.Amount >= StackLimit) continue;

		const int32 AddableAmount = StackLimit - InventoryData.Amount;
		const int32 AmountToAdd = FMath::Min(RemainingAmount, AddableAmount);

		InventoryData.Amount += AmountToAdd;
		RemainingAmount -= AmountToAdd;

		if (RemainingAmount == 0) break;
	}

	for (FInventoryData& InventoryData : Items)
	{
		if (RemainingAmount <= 0)
		{
			break;
		}

		if (IsValid(InventoryData.Item))
		{
			continue;
		}

		const int32 AmountToAdd = FMath::Min(RemainingAmount, StackLimit);
		InventoryData.Item = ItemData;
		InventoryData.Amount = AmountToAdd;

		RemainingAmount -= AmountToAdd;
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItem(UItemDataAsset* ItemData, int32 Amount)
{
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::RemoveItem - ItemData가 유효하지 않습니다."));
		return false;
	}
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::RemoveItem - 1보다 작은 개수의 아이템을 제거할 수 없습니다."));
		return false;
	}

	if (GetItemAmount(ItemData) < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::RemoveItem - 제거하고자 하는 아이템의 수량이 보유한 수량보다 많습니다."));
		return false;
	}

	int32 RemainingAmount = Amount;

	for (int32 Index = Items.Num() - 1; Index >= 0 && RemainingAmount > 0; --Index)
	{
		if (Items[Index].Item != ItemData) continue;

		const int32 AmountToRemove = FMath::Min(RemainingAmount, Items[Index].Amount);
		Items[Index].Amount -= AmountToRemove;
		RemainingAmount -= AmountToRemove;

		if (Items[Index].Amount == 0)
		{
			Items[Index] = FInventoryData();
		}
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::ApplyItemEffect(UItemDataAsset* ItemData)
{
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::ApplyItemEffect - ItemData가 유효하지 않습니다."));
		return false;
	}
	if (!ItemData->UseEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::ApplyItemEffect - %s 아이템의 UseEffectClass가 유효하지 않습니다."), *ItemData->DisplayName.ToString());
		return false;
	}

	UItemUseEffect* UseEffect = NewObject<UItemUseEffect>(this, ItemData->UseEffectClass);

	if (!IsValid(UseEffect))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - %s 아이템의 UseEffect 캐스팅에 실패했습니다."), *ItemData->DisplayName.ToString());
		return false;
	}

	return UseEffect->ApplyEffect(GetOwner(), ItemData);
}

bool UInventoryComponent::UseItem(UItemDataAsset* ItemData)
{
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - ItemData가 유효하지 않습니다."));
		return false;
	}

	if (!HasItem(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - %s 아이템을 보유하고 있지 않습니다."), *ItemData->DisplayName.ToString());
		return false;
	}

	if (!ApplyItemEffect(ItemData)) {
		return false;
	}

	if (ItemData->bConsumable)
	{
		return RemoveItem(ItemData, 1);
	}

	return true;
}

bool UInventoryComponent::UseItemAtIndex(int32 Index)
{
	if (!Items.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItemAtIndex - 유효하지 않은 인덱스입니다. Index=%d"), Index);
		return false;
	}

	FInventoryData& InventoryData = Items[Index];

	if (!IsValid(InventoryData.Item) || InventoryData.Amount <= 0) return false;

	UItemDataAsset* ItemData = InventoryData.Item;

	if (!ApplyItemEffect(ItemData)) return false;

	if (ItemData->bConsumable)
	{
		--InventoryData.Amount;

		if (InventoryData.Amount <= 0)
		{
			InventoryData = FInventoryData();
		}
		OnInventoryChanged.Broadcast();
	}

	return true;
}

UItemDataAsset* UInventoryComponent::GetItemAtIndex(int32 Index) const
{
	if (!Items.IsValidIndex(Index))
	{
		return nullptr;
	}

	const FInventoryData& InventoryData = Items[Index];

	if (!IsValid(InventoryData.Item) || InventoryData.Amount <= 0)
	{
		return nullptr;
	}

	return InventoryData.Item;
}

bool UInventoryComponent::RemoveItemAtIndex(int32 Index, int32 Amount)
{
	if (!Items.IsValidIndex(Index) || Amount <= 0)
	{
		return false;
	}

	FInventoryData& InventoryData = Items[Index];
	if (!IsValid(InventoryData.Item) || InventoryData.Amount < Amount)
	{
		return false;
	}

	InventoryData.Amount -= Amount;

	if (InventoryData.Amount <= 0)
	{
		InventoryData = FInventoryData();
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::SwapItemsAtIndices(int32 SourceIndex, int32 TargetIndex)
{
	if (!Items.IsValidIndex(SourceIndex) || !Items.IsValidIndex(TargetIndex)) return false;

	if (!IsValid(Items[SourceIndex].Item) || Items[SourceIndex].Amount <= 0) return false;

	if (SourceIndex == TargetIndex) return true;

	Items.Swap(SourceIndex, TargetIndex);

	OnInventoryChanged.Broadcast();
	return true;
}


TArray<FInventoryData> UInventoryComponent::GetInventoryItems() const
{
	return Items;
}

int32 UInventoryComponent::GetItemAmount(UItemDataAsset* ItemData) const
{
	if (!IsValid(ItemData)) return 0;

	int32 TotalAmount = 0;
	for (const FInventoryData& InventoryData : Items)
	{
		if (InventoryData.Item == ItemData)
		{
			TotalAmount += InventoryData.Amount;
		}
	}

	return TotalAmount;
}

bool UInventoryComponent::HasItem(UItemDataAsset* ItemData, int32 Amount) const
{
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::HasItem - ItemData가 유효하지 않습니다."));
		return false;
	}
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::HasItem - 유효한 검사 개수가 아닙니다."));
		return false;
	}

	return GetItemAmount(ItemData) >= Amount;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Items.Init(FInventoryData(), MaxSlots);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

