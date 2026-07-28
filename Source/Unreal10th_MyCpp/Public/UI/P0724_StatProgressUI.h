// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P0724_StatProgressUI.generated.h"

class UP0724_StatProgressBar;
class AL0727_Player;

UCLASS()
class UNREAL10TH_MYCPP_API UP0724_StatProgressUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(float CurrentValue, float MaxValue);

	UFUNCTION(BlueprintCallable)
	virtual void SetStamina(float CurrentValue, float MaxValue);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UP0724_StatProgressBar> HealthProgressBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UP0724_StatProgressBar> StaminaProgressBar = nullptr;

protected:
	TWeakObjectPtr<AL0727_Player> OwningPlayer = nullptr;
};
