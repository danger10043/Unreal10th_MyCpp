// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StatComponentInterface.h"
#include "Interface/HealthInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "Interface/StaminaInterface.h"
#include "TestEnemy.generated.h"

class UWidgetComponent;
class UStatComponent;
class AActionPlayer;
class UWeaponDataAsset;
class AWeaponActor;

UCLASS()
class UNREAL10TH_MYCPP_API ATestEnemy : 
	public ACharacter, 
	public IStatComponentInterface, 
	public IWeaponUserInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStatComponent* GetStatComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> RightHandMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> StatUIComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<AActionPlayer> TargetPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TWeakObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UWeaponDataAsset> CurrentWeaponData = nullptr;

	UFUNCTION(BlueprintCallable)
	void SpawnWeaponActor();

	virtual void EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData) override;

public:
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	virtual void OnWeaponAttackState(bool bEnable) override;

	virtual FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() override {
		return OnOnWeaponAttackStateChanged;
	};

	UFUNCTION(BlueprintCallable)
	virtual void AttackFunction();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	FTimerHandle AttackCooldownTimer;

	FVector InitialLocation = FVector(0.0f, 0.0f, 0.0f);
	FRotator InitialRotation = FRotator(0.0f, 0.0f, 0.0f);

private:

	FOnWeaponAttackStateChanged OnOnWeaponAttackStateChanged;

};
