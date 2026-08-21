// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemUseEffect.generated.h"

class UItemDataAsset;

UCLASS(Abstract, Blueprintable, BlueprintType)
class UNREAL10TH_MYCPP_API UItemUseEffect : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Use")
	bool ApplyEffect(AActor* User, const UItemDataAsset* ItemData);

	virtual bool ApplyEffect_Implementation(AActor* User, const UItemDataAsset* ItemData);

	
};
