// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StatComponentInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "EnemyCharacter.generated.h"

class UWeaponDataAsset;
class UAnimMontage;
class AItemWeapon;
class UItemDataAsset;
class UWidgetComponent;
class AActionPlayer;

USTRUCT(BlueprintType)
struct FItemDropEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UItemDataAsset> ItemDataAsset = nullptr;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		meta = (ClampMin = "0.0", ClampMax = "1.0")
	)
	float Probability = 0.0f;
};

UCLASS()
class UNREAL10TH_MYCPP_API AEnemyCharacter : 
	public ACharacter,
	public IStatComponentInterface,
	public IWeaponUserInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> RightHandMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> StatUIComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<AActionPlayer> TargetPlayer = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponComponent> WeaponComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Drop")
	TArray<FItemDropEntry> ItemDropTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death")
	TObjectPtr<UAnimMontage> DieAnimation = nullptr;


	bool bIsDead = false;

	void OnDieAnimationEnded(UAnimMontage* Montage, bool bInterrupted);

	void SpawnDropItemAndDestroy();

protected:
	UFUNCTION(BlueprintCallable)
	virtual void AttackFunction();

	UFUNCTION(BlueprintCallable)
	virtual void DropItems();

	FTimerHandle AttackCooldownTimer;

	FVector InitialLocation = FVector(0.0f, 0.0f, 0.0f);
	FRotator InitialRotation = FRotator(0.0f, 0.0f, 0.0f);

public:
	UFUNCTION()
	virtual void OnDie();

	virtual UStatComponent* GetStatComponent() const override;

	virtual void EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData) override;

	virtual UWeaponComponent* GetWeaponComponent() const override
	{
		return WeaponComponent;
	};

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

};
