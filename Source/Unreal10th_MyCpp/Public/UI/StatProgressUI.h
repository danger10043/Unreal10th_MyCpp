// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatProgressUI.generated.h"

class UStatProgressBar;
class AActionPlayer;
class IStatComponentInterface;

UCLASS()
class UNREAL10TH_MYCPP_API UStatProgressUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(float CurrentValue, float MaxValue);

	UFUNCTION(BlueprintCallable)
	virtual void SetStamina(float CurrentValue, float MaxValue);

	void SetOwnerActor(IStatComponentInterface* InOwner);

public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UStatProgressBar> HealthProgressBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UStatProgressBar> StaminaProgressBar = nullptr;

public:
	IStatComponentInterface* OwningPlayer = nullptr;
};
