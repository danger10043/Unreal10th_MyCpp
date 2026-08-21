// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

class AItemBase;
class UWeaponDataAsset;
class UStaticMesh;
class USkeletalMesh;
class UTexture2D;
class UItemUseEffect;
class UNiagaraSystem;

UCLASS()
class UNREAL10TH_MYCPP_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity")
	FName ItemId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity", meta = (MultiLine = "true"))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<AItemBase> ItemClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Inventory")
	bool bStackable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Inventory", meta = (ClampMin = "1", EditCondition = "bStackable"))
	int32 MaxStackAmount = 999;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Inventory")
	bool bConsumable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Use Function")
	TSubclassOf<UItemUseEffect> UseEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Use Function")
	TSoftObjectPtr<UNiagaraSystem> UseNiagaraSystem;

	// ItemWeapon 에서 사용하는 무기 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

	// 체력 회복 또는 스태미너 회복이 적용할 수치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stat")
	float StatAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item Appearance")
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item Appearance")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item Appearance")
	TSoftObjectPtr<UNiagaraSystem> IdleNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item Appearance")
	TSoftObjectPtr<UNiagaraSystem> PickupNiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item Appearance")
	FVector MeshScale = FVector::OneVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Item Appearance")
	FRotator MeshRotation = FRotator::ZeroRotator;

	// 아이템 생성 위치에 추가할 오프셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Spawn")
	FVector SpawnOffset = FVector::ZeroVector;
};
