// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPoolSubSystem.generated.h"

class AActor;
class APawn;
class UPoolDataAsset;

USTRUCT()
struct FUsingPoolObject
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Actor = nullptr;

	uint64 AcquireSequence = 0;
};

USTRUCT()
struct FObjectPoolBucket
{
	GENERATED_BODY()

	// 아직 사용하지 않는 Actor들을 담아두는 배열
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> ReadyObjects;

	// 현재 사용 중인 Actor들을 담아두는 배열
	UPROPERTY(Transient)
	TArray<FUsingPoolObject> UsingObjects;

	uint64 NextAcquireSequence = 0;
};

UCLASS()
class UNREAL10TH_MYCPP_API UObjectPoolSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	// Pool 에서 Actor를 꺼내거나, 없으면 새로 Spawn 하는 함수
	AActor* Acquire(
		const UPoolDataAsset* PoolData,
		const FTransform& SpawnTransform,
		AActor* Owner = nullptr,
		APawn* Instigator = nullptr
	);

	// 사용을 끝낸 Actor를 Pool에 반환하는 함수
	bool Release(AActor* Actor);

	// Pool을 들어갈 Actor를 미리 생성하두는 함수

	void Prewarm(
		TSubclassOf<AActor> ActorClass,
		int32 Count
	);

private:
	bool IsPoolableClass(TSubclassOf<AActor> ActorClass) const;

private:
	// Key : Actor의 클래스 / Value : 해당 클래스 전용 Pool
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, FObjectPoolBucket> Pools;
};
