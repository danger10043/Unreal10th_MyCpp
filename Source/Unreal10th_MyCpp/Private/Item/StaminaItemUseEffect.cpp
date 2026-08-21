
#include "Item/StaminaItemUseEffect.h"

#include "Component/StatComponent.h"
#include "Data/ItemDataAsset.h"
#include "Interface/StaminaInterface.h"
#include "Interface/StatComponentInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

bool UStaminaItemUseEffect::ApplyEffect_Implementation(AActor* User, const UItemDataAsset* ItemData)
{
	if (!IsValid(User))
	{
		UE_LOG(LogTemp, Warning, TEXT("StaminaItemUseEffect::ApplyEffect - 사용자가 유효하지 않습니다."));
		return false;
	}
	if (!IsValid(ItemData))
	{	
		UE_LOG(LogTemp, Warning, TEXT("StaminaItemUseEffect::ApplyEffect - ItemData가 유효하지 않습니다."));
		return false;
	}
	if (ItemData->StatAmount <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("StaminaItemUseEffect::ApplyEffect - 회복량이 유효하지 않습니다."));
		return false;
	}

	IStatComponentInterface* StatOwner = Cast<IStatComponentInterface>(User);

	if (!StatOwner)
	{
		return false;
	}

	UStatComponent* StatComponent = StatOwner->GetStatComponent();
	
	if (!IsValid(StatComponent) || !StatComponent->GetClass()->ImplementsInterface(UStaminaInterface::StaticClass()))
	{
		return false;
	}

	const float CurrentStamina = IStaminaInterface::Execute_GetCurrentStamina(StatComponent);

	const float MaxStamina = IStaminaInterface::Execute_GetMaxStamina(StatComponent);

	if (CurrentStamina >= MaxStamina)
	{
		return false;
	}

	IStaminaInterface::Execute_RecoveryStamina(StatComponent, ItemData->StatAmount);

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
