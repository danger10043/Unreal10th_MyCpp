// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P0723_HealPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class UNREAL10TH_MYCPP_API AP0723_HealPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AP0723_HealPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	// Health Values
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthTickInterval = 0.2f;

	// Pickup Item Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh = nullptr;

	// Health Timer
	FTimerHandle HealthTickTimer;

};
