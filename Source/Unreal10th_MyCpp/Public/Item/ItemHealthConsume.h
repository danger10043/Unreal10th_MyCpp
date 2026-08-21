// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "ItemHealthConsume.generated.h"

class UItemDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API AItemHealthConsume : public AItemBase
{
	GENERATED_BODY()
	
public:
	virtual void InitializeFromItemData(UItemDataAsset* InItemData) override;

protected:
	virtual void PickUpItem(AActor* InActor) override;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Item|Health",
		meta = (ClampMin = "0.0")
	)
	float HealAmount = 30.0f;
};
