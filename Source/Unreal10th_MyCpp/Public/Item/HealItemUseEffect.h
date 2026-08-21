// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemUseEffect.h"
#include "HealItemUseEffect.generated.h"

UCLASS()
class UNREAL10TH_MYCPP_API UHealItemUseEffect : public UItemUseEffect
{
	GENERATED_BODY()
	
public:
	virtual bool ApplyEffect_Implementation(AActor* User, const UItemDataAsset* ItemData) override;

};
