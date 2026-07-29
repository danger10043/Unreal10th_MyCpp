// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/StaminaInterface.h"
#include "../Interface/HealthInterface.h"
#include "StatComponent.generated.h"

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
	RollCost,
	AttackCost,
};

//DECLARE_DELEGATE(FuncName);  함수 포인터 사용
//DECLARE_DYNAMIC_DELEGATE(FuncName); 리플랙션 시스템 사용(문자열을 키로 하는 테이블).블루프린트에서 사용가능하지만 느리다.DYNAMIC이 붙은 델리게이트에는 무조건 UFUNCTION이 붙은 함수만 사용 가능하다.
//
//DECLARE_MULTICAST_DELEGATE(FuncName); 바인딩 시킬 함수를 여러개 저장 가능.
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FuncName);
//
//DECLARE_MULTICAST_DELEGATE_TwoParams(FuncName, int, float);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FuncName, int, Data); // 블루프린트에서는 변수명이 보여야 하기 때문에 변수명도 지정.
//
//DECLARE_DELEGATE_RetVal(int, FuncName); // 리턴을 int로 하는 Delegate
//DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FuncName, int, Data) // 리턴이 float 이고 parameter가 int Data

// Event
// - 델리게이트와 거의 같음
// - 기본적으로 멀티캐스트(listener가 여러명)
// - 리턴값이 무조건 없다.
// - 클래스 밖에서 호출할 수 없다.
// - C++ 전용
// DECLARE_EVENT(AAcotr, FuncName)


DECLARE_DELEGATE(FOnRunEnd);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatEmpty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChange, float, Current, float, Max);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREAL10TH_MYCPP_API UStatComponent : 
	public UActorComponent, 
	public IStaminaInterface, 
	public IHealthInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatComponent();

	// Stamina Interface Function
	virtual float GetCurrentStamina_Implementation() const override;

	virtual float GetMaxStamina_Implementation() const override;

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

	// Delegate 함수
public:
	UPROPERTY(BlueprintAssignable, Category = "Stat|Stamina")
	FOnStatEmpty OnStaminaEmpty;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Health")
	FOnStatEmpty OnDie;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Stamina")
	FOnStatChange OnStaminaChange;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Health")
	FOnStatChange OnHealthChange;


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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float AttackStaminaCost = 5.0f;

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
