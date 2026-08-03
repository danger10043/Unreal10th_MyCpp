// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/WeaponUserInterface.h"
#include "WeaponComponent.generated.h"

class AWeaponActor;
class UWeaponDataAsset;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREAL10TH_MYCPP_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// 현재 장비 중인 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TWeakObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UWeaponDataAsset> CurrentWeaponData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UWeaponDataAsset> BasicWeaponData = nullptr;

public:
	AWeaponActor* GetCurrentWeapon() const { return CurrentWeapon.Get(); }

	UWeaponDataAsset* GetCurrentWeaponData() const { return CurrentWeaponData; }

	UWeaponDataAsset* GetBasicWeaponData() const { return BasicWeaponData; }

	UAnimMontage* GetAttackMontage() const;

	// Returns false when there is no equipped weapon. A limited weapon falls back
	// to BasicWeaponData after its final use.
	bool ConsumeWeaponUse();

public:
	// 무기 공격 활성화/비활성화 Notify를 받는 Delegate
	FOnWeaponAttackStateChanged OnWeaponAttackStateChanged;
	
	// 
	UFUNCTION(BlueprintCallable)
	void SpawnWeaponActor();

	virtual void EquipWeapon(UWeaponDataAsset* InWeaponData);
};
