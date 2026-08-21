// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/HealItemUseEffect.h"
#include "Component/StatComponent.h"
#include "Data/ItemDataAsset.h"
#include "Interface/HealthInterface.h"
#include "Interface/StatComponentInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

bool UHealItemUseEffect::ApplyEffect_Implementation(AActor* User, const UItemDataAsset* ItemData)
{
	if (!IsValid(User))
	{
		UE_LOG(LogTemp, Warning, TEXT("HealItemUseEffect::ApplyEffect - 사용자가 유효하지 않습니다."));
		return false;
	}
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("HealItemUseEffect::ApplyEffect - 사용자가 유효하지 않습니다."));
		return false;
	}
	if (ItemData->StatAmount <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealItemUseEffect::ApplyEffect - 회복량이 유효하지 않습니다."));
		return false;
	}

	IStatComponentInterface* StatOwner = Cast<IStatComponentInterface>(User);
	if (!StatOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealItemUseEffect::ApplyEffect - 사용자가 StatComponent를 가지고 있지 않습니다."));
		return false;
	}

	UStatComponent* StatComponent = StatOwner->GetStatComponent();
	if (!IsValid(StatComponent) || !StatComponent->GetClass()->ImplementsInterface(UHealthInterface::StaticClass()))
	{
		return false;
	}

	const float CurrentHealth = IHealthInterface::Execute_GetCurrentHealth(StatComponent);
	const float MaxHealth = IHealthInterface::Execute_GetMaxHealth(StatComponent);

	if (CurrentHealth >= MaxHealth)
	{
		return false;
	}

	IHealthInterface::Execute_HealHealth(StatComponent, ItemData->StatAmount);

	if (!ItemData->UseNiagaraSystem.IsNull())
	{
		UNiagaraSystem* LoadedNiagaraSystem = ItemData->UseNiagaraSystem.LoadSynchronous();

		if (IsValid(LoadedNiagaraSystem))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				User,
				LoadedNiagaraSystem,
				User->GetActorLocation(),
				User->GetActorRotation()
			);
		}
	}

	return true;
}
