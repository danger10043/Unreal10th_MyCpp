// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatingActor.h"
#include "FloatingActorByCurve.generated.h"

UCLASS()
class UNREAL10TH_MYCPP_API AFloatingActorByCurve : public AFloatingActor
{
	GENERATED_BODY()
	
protected:
	virtual void TickMovement(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	UCurveFloat* Curve = nullptr;
};
