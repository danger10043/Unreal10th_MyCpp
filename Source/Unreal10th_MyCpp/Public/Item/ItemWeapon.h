// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/ItemBase.h"
#include "ItemWeapon.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UWeaponDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API AItemWeapon : public AItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemWeapon();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

	virtual void PickUpItem(AActor* InActor) override;

protected:
	FTimerHandle PickupEffectTimerHandle;

	UPROPERTY(EditAnywhere)
	UCurveFloat* LocationCurve = nullptr;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* ScaleCurve = nullptr;

	float PickupElapsedTime;

	FVector PickupStartLocation;

	TWeakObjectPtr<AActor> PickupTarget;
};
