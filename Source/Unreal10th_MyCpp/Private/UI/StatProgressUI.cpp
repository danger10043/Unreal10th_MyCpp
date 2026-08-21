// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatProgressUI.h"
#include "UI/StatProgressBar.h"
#include "Interface/StatComponentInterface.h"
#include "Player/ActionPlayer.h"
#include "Component/StatComponent.h"
#include "Components/WidgetComponent.h"


void UStatProgressUI::NativeConstruct()
{
	Super::NativeConstruct();
	APlayerController* TargetPC = GetWorld()->GetFirstPlayerController();
	UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(GetOuter());

	if (OwningPlayer)
	{
		if (IsValid(HealthProgressBar))
		{
			HealthProgressBar->SetProgressBarColor(FLinearColor(1.0f, 0.1f, 0.1f, 1.0f));
		}

		if (IsValid(StaminaProgressBar))
		{
			StaminaProgressBar->SetProgressBarColor(FLinearColor(0.1f, 1.0f, 0.1f, 1.0f));
		}

		UStatComponent* StatComponent = OwningPlayer->GetStatComponent();
		StatComponent->OnHealthChange.AddDynamic(this, &UStatProgressUI::SetHealth);
		StatComponent->OnStaminaChange.AddDynamic(this, &UStatProgressUI::SetStamina);

		float MaxHealth = StatComponent->IHealthInterface::Execute_GetMaxHealth(StatComponent);
		float MaxStamina = StatComponent->IStaminaInterface::Execute_GetMaxStamina(StatComponent);
		SetHealth(MaxHealth, MaxHealth);
		SetStamina(MaxStamina, MaxStamina);
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("뭐여 왼손잡이여?"));
		UE_LOG(LogTemp, Warning, TEXT("Widget Outer : %s"), *GetOuter()->GetName());
	}
}

void UStatProgressUI::SetHealth(float CurrentValue, float MaxValue)
{
	if (IsValid(HealthProgressBar))
	{
		HealthProgressBar->SetProgressBarValue(CurrentValue, MaxValue);
	}
}

void UStatProgressUI::SetStamina(float CurrentValue, float MaxValue)
{
	if (IsValid(StaminaProgressBar))
	{
		StaminaProgressBar->SetProgressBarValue(CurrentValue, MaxValue);
	}
}

void UStatProgressUI::SetOwnerActor(IStatComponentInterface* InOwner)
{
	OwningPlayer = InOwner;
}