// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/L0723_StatComponent.h"

// Sets default values for this component's properties
UL0723_StatComponent::UL0723_StatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UL0723_StatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina = MaxStamina;
	CurrentHealth = MaxHealth;

	UWorld* World = GetWorld();
	FTimerManager& TimerManager = World->GetTimerManager();

	TimerManager.SetTimer(
		StaminaRecoveryTickTimer,
		FTimerDelegate::CreateLambda(
			[this]()
			{
				Execute_RecoveryStamina(this, RecoveryStaminaAmount);
			}),
		RecoveryStaminaInterval,
		true
	);
}

float UL0723_StatComponent::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

float UL0723_StatComponent::GetMaxStamina_Implementation() const
{
	return MaxStamina;
}

bool UL0723_StatComponent::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;
	if (CurrentStamina >= InAmount)
	{
		CurrentStamina -= InAmount;
		bResult = true;
		StopRecover();
	}

	OnStaminaChange.Broadcast(CurrentStamina, MaxStamina); // 블루프린트 디스패쳐 Call 과 같다

	if (FMath::IsNearlyZero(CurrentStamina))
	{
		OnStaminaEmpty.Broadcast();
	}
	return bResult;
}

void UL0723_StatComponent::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0.0f, MaxStamina);
	OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);
}

float UL0723_StatComponent::GetCurrentHealth_Implementation() const
{
	return CurrentHealth;
}

float UL0723_StatComponent::GetMaxHealth_Implementation() const
{
	return MaxHealth;
}

bool UL0723_StatComponent::DamageHealth_Implementation(float InAmount)
{
	bool bResult = false;
	if (CurrentHealth > InAmount)
	{
		CurrentHealth -= InAmount;
		bResult = true;
	}
	else
	{
		CurrentHealth = 0.0f;
		bResult = false;
	}

	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
	if (FMath::IsNearlyZero(CurrentHealth))
	{
		OnDie.Broadcast();
	}

	return bResult;
}

void UL0723_StatComponent::HealHealth_Implementation(float InAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + InAmount, 0.0f, MaxHealth);
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
}

void UL0723_StatComponent::OnRunStart()
{	
	UWorld* World = GetWorld();
	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.PauseTimer(StaminaRecoveryTickTimer);

	TimerManager.SetTimer(
		StaminaRunCostTimer,
		FTimerDelegate::CreateLambda(
			[this]()
			{
				if (!IStaminaInterface::Execute_ConsumeStamina(this, RunStaminaCost))
				{
					OnRunEnd();
					if (RunEndFunc.IsBound())
					{ 
						RunEndFunc.Execute();
					}
				}
			}),
		RunStaminaInterval,
		true
	);
}

void UL0723_StatComponent::OnRunEnd()
{
	UWorld* World = GetWorld();
	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.PauseTimer(StaminaRunCostTimer);
	StopRecover();
}

void UL0723_StatComponent::StopRecover()
{
	UWorld* World = GetWorld();
	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.PauseTimer(StaminaRecoveryTickTimer);

	TimerManager.SetTimer(
		StaminaRecoveryCooldownTimer,
		FTimerDelegate::CreateLambda(
			[this]()
			{
				UWorld* TempWorld = GetWorld();
				FTimerManager& TempTimerManager = TempWorld->GetTimerManager();
				TempTimerManager.UnPauseTimer(StaminaRecoveryTickTimer);
			}
		),
		RecoveryStaminaCooldown,
		false
	);
}


// Called every frame
void UL0723_StatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FTimerHandle& UL0723_StatComponent::GetStaminaTimerHandle(EStaminaTimer Type)
{
	switch (Type)
	{
	case EStaminaTimer::RecoveryCooldown:
		return StaminaRecoveryCooldownTimer;

	case EStaminaTimer::RecoveryTick:
		return StaminaRecoveryTickTimer;

	case EStaminaTimer::RunCostTick:
		return StaminaRunCostTimer;

	default:
		return StaminaRecoveryCooldownTimer;
	}
}

float UL0723_StatComponent::GetStaminaValue(EStaminaValueType Type) const
{
	switch (Type)
	{
	case EStaminaValueType::Current:
		return CurrentStamina;

	case EStaminaValueType::Max:
		return MaxStamina;

	case EStaminaValueType::RecoveryCooldown:
		return RecoveryStaminaCooldown;

	case EStaminaValueType::RecoveryAmount:
		return RecoveryStaminaAmount;

	case EStaminaValueType::RecoveryTickInterval:
		return RecoveryStaminaInterval;

	case EStaminaValueType::RunTickInterval:
		return RunStaminaInterval;

	case EStaminaValueType::RunTickCost:
		return RunStaminaCost;

	case EStaminaValueType::RollCost:
		return RollStaminaCost;

	case EStaminaValueType::AttackCost:
		return AttackStaminaCost;

	default:
		return 0.0f;
	}
}

void UL0723_StatComponent::SetStaminaValue(EStaminaValueType Type, float InValue)
{
	switch (Type)
	{
	case EStaminaValueType::Current:
		CurrentStamina = InValue;

	case EStaminaValueType::Max:
		MaxStamina = InValue;

	case EStaminaValueType::RecoveryCooldown:
		RecoveryStaminaCooldown = InValue;

	case EStaminaValueType::RecoveryAmount:
		RecoveryStaminaAmount = InValue;

	case EStaminaValueType::RecoveryTickInterval:
		RecoveryStaminaInterval = InValue;

	case EStaminaValueType::RunTickInterval:
		RunStaminaInterval = InValue;

	case EStaminaValueType::RunTickCost:
		RunStaminaCost = InValue;

	case EStaminaValueType::RollCost:
		RollStaminaCost = InValue;

	}
}

