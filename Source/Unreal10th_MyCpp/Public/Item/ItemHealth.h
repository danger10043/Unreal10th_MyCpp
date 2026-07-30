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

	// Health Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthTickInterval = 0.2f;

public:
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

};