// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P0724_StatProgressBar.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class UNREAL10TH_MYCPP_API UP0724_StatProgressBar : public UUserWidget
{
	GENERATED_BODY()

	// UI Functions
public:
	UFUNCTION(BlueprintCallable)
	void SetProgressBarColor(FLinearColor InColor);

	UFUNCTION(BlueprintCallable)
	void SetProgressBarValue(float CurrentValue, float MaxValue);

	// UI Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> ActorProgressBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentValueText = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxValueText = nullptr;
};
