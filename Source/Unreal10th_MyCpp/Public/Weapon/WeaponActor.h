// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/WeaponUserInterface.h"
#include "WeaponActor.generated.h"

class UBoxComponent;
class UWeaponDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponActor();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnEquipped(AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	void OnHitAreaBeginOverlap(
		UPrimitiveComponent* InOnComponentBeginOverlap,
		AActor* InOtherActor,
		UPrimitiveComponent* InOtherComp,
		int32 InOtherBodyIndex,
		bool bFromSweep,
		const FHitResult& InSweepResult
	);

	UFUNCTION(BlueprintCallable)
	void InitializeWeapon(UWeaponDataAsset* InData);

public:
	UFUNCTION(BlueprintCallable)
	void AttackEnable(bool bEnable);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> HitArea = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachSocketName = TEXT("hand_rSocket");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWeaponDataAsset> WeaponData;

private:
	// 무기를 장비하고 있는 캐릭터
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
};
