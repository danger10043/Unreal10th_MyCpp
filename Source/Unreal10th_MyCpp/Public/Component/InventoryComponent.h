// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItemDataAsset;

USTRUCT(BlueprintType)
struct FInventoryData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemDataAsset> Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0"))
	int32 Amount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREAL10TH_MYCPP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemDataAsset* ItemData, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemDataAsset* ItemData, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(UItemDataAsset* ItemData);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItemAtIndex(int32 Index);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UItemDataAsset* GetItemAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemAtIndex(int32 Index, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapItemsAtIndices(int32 SourceIndex, int32 TargetIndex);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FInventoryData> GetInventoryItems() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemAmount(UItemDataAsset* ItemData) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(UItemDataAsset* ItemData, int32 Amount = 1) const;

	const TArray<FInventoryData>& GetItems() const { return Items; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:
	bool ApplyItemEffect(UItemDataAsset* ItemData);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 9;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryData> Items;
		
};
