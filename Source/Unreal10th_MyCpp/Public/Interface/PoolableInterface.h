// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

class UObjectPoolSubSystem;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UNREAL10TH_MYCPP_API IPoolableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSpawnFromPool(const FTransform& Transform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnReturnToPool();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UObjectPoolSubSystem* GetObjectPoolSubSystem() const;
};
