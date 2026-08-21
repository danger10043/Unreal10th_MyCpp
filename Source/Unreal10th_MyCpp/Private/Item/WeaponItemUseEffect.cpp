// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WeaponItemUseEffect.h"

#include "Data/ItemDataAsset.h"
#include "Data/WeaponDataAsset.h"
#include "Interface/WeaponUserInterface.h"

bool UWeaponItemUseEffect::ApplyEffect_Implementation(AActor* User, const UItemDataAsset* ItemData)
{
	if (!IsValid(User))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponItemUseEffect::ApplyEffect - 사용자가 유효하지 않습니다."));
		return false;
	}

	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponItemUseEffect::ApplyEffect - ItemData가 유효하지 않습니다."));
		return false;
	}

	if (!IsValid(ItemData->WeaponData))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponItemUseEffect::ApplyEffect - WeaponData가 유효하지 않습니다."));
		return false;
	}

	if (!User->GetClass()->ImplementsInterface(UWeaponUserInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponItemUseEffect::ApplyEffect - 사용자가 무기를 장착할 수 없습니다."));
		return false;
	}

	IWeaponUserInterface::Execute_EquipWeapon(User, ItemData->WeaponData);

	return true;
}
