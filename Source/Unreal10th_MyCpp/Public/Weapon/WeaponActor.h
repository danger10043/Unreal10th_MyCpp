// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/WeaponUserInterface.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;
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
	void EquipToTarget(AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	void DropWeapon();

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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> HitArea = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachSocketName = TEXT("hand_rSocket");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WeaponDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttackCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWeaponDataAsset> WeaponData;

	// 무기가 드랍 된 후 사라질 때까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropLifeSpan = 10.0f;

	// 드랍 직후에 플레이어와 물리 상호작용이 안되는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicsDelay = 0.8f;

private:
	// 무기를 장비하고 있는 캐릭터
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

	// PhysicsDelay용 타이머 핸들
	FTimerHandle PhysicsDelayTimerHandle;
};
