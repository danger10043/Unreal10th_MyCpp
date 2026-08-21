// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ItemFactorySubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Data/ItemDataAsset.h"
#include "Item/ItemBase.h"

AItemBase* UItemFactorySubsystem::SpawnItem(UItemDataAsset* ItemDataAsset)
{
	return SpawnItemAt(ItemDataAsset, FTransform::Identity);
}

AItemBase* UItemFactorySubsystem::SpawnItemAt(
	UItemDataAsset* ItemDataAsset, 
	const FTransform& SpawnTransform,
	const bool bDoLaunchToForwardOfPlayer,
	const FVector LaunchDirection
	)
{
	if (!IsValid(ItemDataAsset) || !ItemDataAsset->ItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemFactorySubsystem::SpawnItemAt - ItemDataAsset 또는 ItemClass가 유효하지 않습니다."));
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemFactorySubsystem::SpawnItemAt - World 가 유효하지 않습니다."));
		return nullptr;
	}

	FTransform FinalTransform = SpawnTransform;
	FinalTransform.AddToTranslation(ItemDataAsset->SpawnOffset);

	AItemBase* SpawnedItem = World->SpawnActorDeferred<AItemBase>(
		ItemDataAsset->ItemClass,
		FinalTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (!IsValid(SpawnedItem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemFactorySubsystem::SpawnItemAt - 아이템 생성을 실패했습니다."));
		return nullptr;
	}

	SpawnedItem->InitializeFromItemData(ItemDataAsset);
	SpawnedItem->PrePareForDropSpawn();

	UGameplayStatics::FinishSpawningActor(
		SpawnedItem,
		FinalTransform
	);

	SpawnedItem->LaunchAsDroppedItem(bDoLaunchToForwardOfPlayer, LaunchDirection);

	return SpawnedItem;
}
