// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"

UCLASS()
class UNREAL10TH_MYCPP_API AFloatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void TickMovement(float DeltaTime);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovePeriod = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveDistance = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MainMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedTime = 0.0f;
};
