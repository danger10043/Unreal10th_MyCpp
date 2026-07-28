// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/P0724_StatProgressUI.h"
#include "UI/P0724_StatProgressBar.h"
#include "Player/L0727_Player.h"
#include "Component/L0723_StatComponent.h"


void UP0724_StatProgressUI::NativeConstruct()
{
	Super::NativeConstruct();
	APlayerController* TargetPC = GetWorld()->GetFirstPlayerController();

	if (TargetPC)
	{
		HealthProgressBar->SetProgressBarColor(FLinearColor(1.0f, 0.1f, 0.1f, 1.0f));
		StaminaProgressBar->SetProgressBarColor(FLinearColor(0.1f, 1.0f, 0.1f, 1.0f));

		OwningPlayer = Cast<AL0727_Player>(TargetPC->GetPawn());
		UL0723_StatComponent* StatComponent = OwningPlayer->GetStatComponent();
		StatComponent->OnHealthChange.AddDynamic(this, &UP0724_StatProgressUI::SetHealth);
		StatComponent->OnStaminaChange.AddDynamic(this, &UP0724_StatProgressUI::SetStamina);

		float MaxHealth = StatComponent->IP0723_HealthInterface::Execute_GetMaxHealth(StatComponent);
		float MaxStamina = StatComponent->IStaminaInterface::Execute_GetMaxStamina(StatComponent);
		SetHealth(MaxHealth, MaxHealth);
		SetStamina(MaxStamina, MaxStamina);
	}
}

void UP0724_StatProgressUI::SetHealth(float CurrentValue, float MaxValue)
{
	HealthProgressBar->SetProgressBarValue(CurrentValue, MaxValue);
}

void UP0724_StatProgressUI::SetStamina(float CurrentValue, float MaxValue)
{
	StaminaProgressBar->SetProgressBarValue(CurrentValue, MaxValue);
}
