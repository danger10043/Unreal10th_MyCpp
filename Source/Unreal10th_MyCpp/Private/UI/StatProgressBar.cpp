// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StatProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UStatProgressBar::SetProgressBarColor(FLinearColor InColor)
{
	ActorProgressBar->SetFillColorAndOpacity(InColor);
}

void UStatProgressBar::SetProgressBarValue(float CurrentValue, float MaxValue)
{
	float Percent = CurrentValue / MaxValue;
	ActorProgressBar->SetPercent(Percent);

	int32 IntCurrentValue = static_cast<int32>(CurrentValue);
	int32 IntMaxValue = static_cast<int32>(MaxValue);

	FText ChangedCurrentValueText = FText::AsNumber(IntCurrentValue);
	FText ChangedMaxValueText = FText::AsNumber(IntMaxValue);

	CurrentValueText->SetText(ChangedCurrentValueText);
	MaxValueText->SetText(ChangedMaxValueText);
}
