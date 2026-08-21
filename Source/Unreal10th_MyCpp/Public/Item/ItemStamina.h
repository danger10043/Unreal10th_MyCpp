// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "ItemStamina.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10TH_MYCPP_API AItemStamina : public AItemBase
{
	GENERATED_BODY()
	
public:
	AItemStamina();

	virtual void InitializeFromItemData(UItemDataAsset* InItemData) override;

	UFUNCTION(BlueprintCallable)
	float GetStamina() const;

	UFUNCTION(BlueprintCallable)
	float GetStaminaTickInterval() const;

	UFUNCTION(BlueprintCallable)
	void SetStamina(float InStamina);

	UFUNCTION(BlueprintCallable)
	void SetStaminaTickInterval(float InStaminaTickInterval);

	UFUNCTION(BlueprintCallable)
	virtual void PickUpItem(AActor* InActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void pickDownItem(AActor* InActor) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Stamina Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stamina = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaTickInterval = 0.2f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* MeshMID = nullptr;

	float CurrentMeshBright = 3.0f;
	float MeshBrightChangeAmount = 3.0f;

	float MaxMeshBright = 150.0f;
	float MinMeshBright = 3.0f;

	FName MIDColorName = TEXT("Color");
	FName MIDBrightnessName = TEXT("Brightness");

	// Stamina Timer
	FTimerHandle StaminaTickTimer;

	FTimerHandle ItemMeshBrightnessIncreaseTimerHandle;

	FTimerHandle ItemMeshBrightnessDecreaseTimerHandle;

	FTimerHandle ItemMeshRotationTimerHandle;
};
