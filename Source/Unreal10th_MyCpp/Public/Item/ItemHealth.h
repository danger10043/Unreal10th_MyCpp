// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBase.h"
#include "GameFramework/Actor.h"
#include "ItemHealth.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class UNREAL10TH_MYCPP_API AItemHealth : public AItemBase
{
	GENERATED_BODY()

protected:
	// Sets default values for this actor's properties
	AItemHealth();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	// Health Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthTickInterval = 0.2f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* MeshMID = nullptr;

	float CurrentMeshBright = 3.0f;
	float MeshBrightChangeAmount = 3.0f;

	float MaxMeshBright = 150.0f;
	float MinMeshBright = 3.0f;

	FName MIDColorName = TEXT("Color");
	FName MIDBrightnessName = TEXT("Brightness");


public:
	virtual void InitializeFromItemData(UItemDataAsset* InItemData) override;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthTickInterval() const;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float InHealth);

	UFUNCTION(BlueprintCallable)
	void SetHealthTickInterval(float InHealthTickInterval);

	UFUNCTION(BlueprintCallable)
	virtual void PickUpItem(AActor* InActor) override;

	UFUNCTION(BlueprintCallable)
	virtual void pickDownItem(AActor* InActor) override;


	// Health Timer
	FTimerHandle HealthTickTimer;

	FTimerHandle ItemMeshBrightnessIncreaseTimerHandle;

	FTimerHandle ItemMeshBrightnessDecreaseTimerHandle;

	FTimerHandle ItemMeshRotationTimerHandle;

};