// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StaminaInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStaminaInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UNREAL10TH_MYCPP_API IStaminaInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stamina")
	float GetCurrentStamina() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stamina")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stamina")
	bool ConsumeStamina(float InAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stamina")
	void RecoveryStamina(float InAmount);

	// 순수 가상 함수 : C++ 전용
	// virtual bool RecoveryStamina(float InAmount) = 0;
};
