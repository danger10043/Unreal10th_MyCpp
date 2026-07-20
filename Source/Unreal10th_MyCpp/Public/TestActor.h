// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// generated.h는 항상 마지막에 include 해야 한다.
#include "TestActor.generated.h"

// UCLASE()
// - C++ 클래스를 언리얼 엔진의 객체 시스템에 등록(= UObject)
// - 가비지 콜렉터 지원, 블루프린트 연동, 에디터에서 노출 등이 가능해짐

UCLASS()
class UNREAL10TH_MYCPP_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
