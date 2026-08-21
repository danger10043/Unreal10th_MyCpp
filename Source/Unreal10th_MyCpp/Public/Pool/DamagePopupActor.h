// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PoolableInterface.h"
#include "DamagePopupActor.generated.h"

class UWidgetComponent;
class UObjectPoolSubSystem;
class UDamageTextWidget;

UCLASS()
class UNREAL10TH_MYCPP_API ADamagePopupActor : 
	public AActor,
	public IPoolableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamagePopupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnSpawnFromPool_Implementation(const FTransform& Transform) override;

	virtual void OnReturnToPool_Implementation() override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> DamagePopupWidgetComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UObjectPoolSubSystem* PoolSubSystem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDamageTextWidget* DamagePopupWidget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float PopupLifetime = 2.0f;

	UFUNCTION()
	void HandlePopupLifetimeEnded();

	FTimerHandle ReturnToPoolTimer;

public:
	void ShowDamage(float InDamage, const FLinearColor& TextColor, const FLinearColor& OutlineColor);

	UObjectPoolSubSystem* GetObjectPoolSubSystem_Implementation() const override;

};
