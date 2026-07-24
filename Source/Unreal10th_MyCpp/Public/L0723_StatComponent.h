// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/StaminaInterface.h"
#include "../Interface/P0723_HealthInterface.h"
#include "L0723_StatComponent.generated.h"

enum class EStaminaTimer
{
	RecoveryCooldown,
	RecoveryTick,
	RunCostTick
};

enum class EStaminaValueType
{
	Current,
	Max,
	RecoveryCooldown,
	RecoveryTickInterval,
	RecoveryAmount,
	RunTickInterval,
	RunTickCost,
	RollCost
};

DECLARE_DELEGATE(FOnRunEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREAL10TH_MYCPP_API UL0723_StatComponent : public UActorComponent, public IStaminaInterface, public IP0723_HealthInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UL0723_StatComponent();

	// Stamina Interface Function
	virtual float GetCurrentStamina_Implementation() const override;

	virtual bool ConsumeStamina_Implementation(float InAmount) override;

	virtual void RecoveryStamina_Implementation(float InAmount) override;

	// Stamina Action Function
	virtual void OnRunStart();

	virtual void OnRunEnd();

	virtual void StopRecover();

	FOnRunEnd RunEndFunc;

	// Health Interface Function
	virtual float GetCurrentHealth_Implementation() const override;

	virtual float GetMaxHealth_Implementation() const override;

	virtual bool DamageHealth_Implementation(float InAmount) override;

	virtual void HealHealth_Implementation(float InAmount) override;

	// Basic Function
protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Stamina Timer Getter Function
public:
	virtual FTimerHandle& GetStaminaTimerHandle(EStaminaTimer Type);

	// Stamina Timer
protected:
	FTimerHandle StaminaRecoveryCooldownTimer;

	FTimerHandle StaminaRecoveryTickTimer;

	FTimerHandle StaminaRunCostTimer;
	
	// Stamina Values
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Stat|Stamina")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RecoveryStaminaCooldown = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RecoveryStaminaAmount = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RecoveryStaminaInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RunStaminaInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RunStaminaCost = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RollStaminaCost = 20.0f;

	// Stamina Values Getter/Setter
public:
	virtual float GetStaminaValue(EStaminaValueType Type) const;
	
	virtual void SetStaminaValue(EStaminaValueType Type, float InValue);

	// Health Values
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Health")
	float MaxHealth = 100.0f;
};
