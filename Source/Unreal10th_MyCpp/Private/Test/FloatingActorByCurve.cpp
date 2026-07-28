// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/FloatingActorByCurve.h"

void AFloatingActorByCurve::TickMovement(float DeltaTime)
{
	float CurveValue = Curve->GetFloatValue(ElapsedTime);
	MainMesh->SetRelativeLocation(FVector(0.0f, 0.0f, CurveValue * MoveDistance));

	ElapsedTime += DeltaTime;
	if (ElapsedTime > 1.5f)
	{
		ElapsedTime = 0.0f;
	}
}
