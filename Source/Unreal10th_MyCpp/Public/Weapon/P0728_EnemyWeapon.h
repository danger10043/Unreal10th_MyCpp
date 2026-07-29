// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/L0728_WeaponUserInterface.h"
#include "P0728_EnemyWeapon.generated.h"

class UBoxComponent;

UCLASS()
class UNREAL10TH_MYCPP_API AP0728_EnemyWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AP0728_EnemyWeapon();

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
	float WeaponDamage = 5.0f;

private:
	// 무기를 장비하고 있는 캐릭터
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
};
