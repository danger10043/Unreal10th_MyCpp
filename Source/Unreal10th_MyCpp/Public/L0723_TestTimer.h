// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "L0723_TestTimer.generated.h"

UCLASS()
class UNREAL10TH_MYCPP_API AL0723_TestTimer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AL0723_TestTimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Test_TimerOnce();

	UFUNCTION(BlueprintCallable)
	void Test_TimerRepeat();
		
	UFUNCTION(BlueprintCallable)
	void Test_TimerDelayRepeat();

	UFUNCTION(BlueprintCallable)
	void Test_TimerLambda();

	UFUNCTION(BlueprintCallable)
	void Test_TimerAllClear();

	void Test_MessagePrint1();
	void Test_MessagePrint2();
	void Test_MessagePrint3();

private:
	FTimerHandle TestOnce;
	FTimerHandle TestRepeat;
	FTimerHandle TestDelayRepeat;
	FTimerHandle TestLambda;

};
