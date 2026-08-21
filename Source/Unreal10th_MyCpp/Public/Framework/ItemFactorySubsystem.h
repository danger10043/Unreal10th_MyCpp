// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ItemFactorySubsystem.generated.h"

class AItemBase;
class UItemDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API UItemFactorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Item Factory")
	AItemBase* SpawnItem(UItemDataAsset* ItemDataAsset);

	UFUNCTION(BlueprintCallable, Category = "Item Factory")
	AItemBase* SpawnItemAt(
		UItemDataAsset* ItemDataAsset,
		const FTransform& SpawnTransform,
		const bool bDoLaunchToForwardOfPlayer = false,
		const FVector LaunchDirection = FVector(0, 0, 0)
	);
};
