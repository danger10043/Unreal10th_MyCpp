// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "P0723_HealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UP0723_HealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UNREAL10TH_MYCPP_API IP0723_HealthInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	bool DamageHealth(float InAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
	void HealHealth(float InAmount);
};
