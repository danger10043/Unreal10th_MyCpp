// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ObjectPoolSubSystem.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Interface/PoolableInterface.h"
#include "Data/PoolDataAsset.h"

void UObjectPoolSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UObjectPoolSubSystem::Deinitialize()
{
	for (TPair<TObjectPtr<UClass>, FObjectPoolBucket>& Pair : Pools)
	{
		FObjectPoolBucket& Bucket = Pair.Value;

		for (AActor* Actor : Bucket.ReadyObjects)
		{
			if (IsValid(Actor))
			{
				Actor->Destroy();
			}
		}

		for (const FUsingPoolObject& UsingPoolObject : Bucket.UsingObjects)
		{
			if (IsValid(UsingPoolObject.Actor))
			{
				UsingPoolObject.Actor->Destroy();
			}
		}
	}

	Pools.Empty();

	Super::Deinitialize();
}

AActor* UObjectPoolSubSystem::Acquire(
	const UPoolDataAsset* PoolData,
	const FTransform& SpawnTransform, 
	AActor* Owner, 
	APawn* Instigator)
{
	if (!PoolData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Acquire - PoolData가 존재하지 않습니다."));
		return nullptr;
	}

	if (!IsPoolableClass(PoolData->PooledActorClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Acquire - %s가 Poolable하지 않습니다."), PoolData->PooledActorClass ? *PoolData->PooledActorClass->GetName() : TEXT("NULL"));
		return nullptr;
	}

	FObjectPoolBucket& Bucket = Pools.FindOrAdd(PoolData->PooledActorClass.Get());

	AActor* PooledActor = nullptr;

	// 1. Ready 된 Actor가 있으면 항상 우선 사용
	if (Bucket.ReadyObjects.Num() > 0)
	{
		PooledActor = Bucket.ReadyObjects.Pop(EAllowShrinking::No);
		UE_LOG(LogTemp, Log, TEXT("UObjectPoolSubSystem::Acquire - %s 를 Pool 에서 뽑아 활성화했습니다."), *PooledActor->GetName());
	}
	else
	{
		switch (PoolData->AcquireOption)
		{
			// Case 1 - DoNotSpawn (Pool에 사용 가능한 Actor가 없으면 생성 요청을 무시한다.)
		case EPoolAcquireOption::DoNotSpawn:
		{
			UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Acquire - 가용한 Actor가 없으므로 Actor를 생성하지 않았습니다."));
			return nullptr;
		}

		// Case 2 - Grow (Pool에 사용 가능한 Actor 가 없으면 Actor를 새로 Spawn하여 충당한다.)
		case EPoolAcquireOption::Grow:
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Owner;
			SpawnParams.Instigator = Instigator;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			PooledActor = GetWorld()->SpawnActor<AActor>(
				PoolData->PooledActorClass,
				SpawnTransform,
				SpawnParams
			);

			if (PooledActor)
			{
				UE_LOG(LogTemp, Log, TEXT("UObjectPoolSubSystem::Acquire - %s 를 생성하여 활성화했습니다."), *PooledActor->GetName());
			}
			break;
		}

		// Case 3 - ReuseOldest (Pool에 사용 가능한 Actor 가 없으면 사용 중인 Actor중 가장 오래된 것을 초기화하여 재사용한다.)
		case EPoolAcquireOption::ReuseOldest:
		{
				if (Bucket.UsingObjects.IsEmpty())
				{
					return nullptr;
				}

				int32 OldestIndex = 0;
				for (int32 Index = 1; Index < Bucket.UsingObjects.Num(); ++Index)
				{
					if (Bucket.UsingObjects[Index].AcquireSequence < Bucket.UsingObjects[OldestIndex].AcquireSequence)
					{
						OldestIndex = Index;
					}
				}
				PooledActor = Bucket.UsingObjects[OldestIndex].Actor;
				Bucket.UsingObjects.RemoveAtSwap(OldestIndex);
				IPoolableInterface::Execute_OnReturnToPool(PooledActor);

				UE_LOG(LogTemp, Log, TEXT("UObjectPoolSubSystem::Acquire - 실행중인 %s 를 가져와 초기화하여 다시 활성화했습니다."), *PooledActor->GetName());

				break;
		}
		}
	}

	if (!IsValid(PooledActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Acquire - %s를 Acquire 을 실패했습니다."), PoolData->PooledActorClass ? *PoolData->PooledActorClass->GetName() : TEXT("NULL"));
		return nullptr;
	}

	PooledActor->SetOwner(Owner);
	PooledActor->SetInstigator(Instigator);

	Bucket.UsingObjects.Add(
		FUsingPoolObject{
			PooledActor,
			++Bucket.NextAcquireSequence
		}
	);

	IPoolableInterface::Execute_OnSpawnFromPool(
		PooledActor,
		SpawnTransform
	);

	return PooledActor;
}

bool UObjectPoolSubSystem::Release(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Release - Actor가 유효하지 않습니다."));
		return false;
	}

	if (!Actor->Implements<UPoolableInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Release - %s가 Poolable하지 않습니다."), *Actor->GetName());
		return false;
	}

	FObjectPoolBucket* Bucket = Pools.Find(Actor->GetClass());

	if (!Bucket)
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Release - %s에 대한 Pool이 존재하지 않습니다."), *Actor->GetName());
		return false;
	}

	const int32 UsingIndex = Bucket->UsingObjects.IndexOfByPredicate(
		[Actor](const FUsingPoolObject& Entry)
		{
			return Entry.Actor == Actor;
		}
	);
	if (UsingIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Release - %s가 UsingObjects 에 존재하지 않습니다."), *Actor->GetName());
		return false;
	}
	Bucket->UsingObjects.RemoveAtSwap(UsingIndex);
	IPoolableInterface::Execute_OnReturnToPool(Actor);
	Bucket->ReadyObjects.Add(Actor);

	UE_LOG(LogTemp, Log, TEXT("UObjectPoolSubSystem::Release - %s 를 Pool에 넣고 비활성화했습니다."), *Actor->GetName());

	return true;
}

void UObjectPoolSubSystem::Prewarm(TSubclassOf<AActor> ActorClass, int32 Count)
{
	if (!IsPoolableClass(ActorClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Prewarm - %s가 Poolable하지 않습니다."), ActorClass ? *ActorClass->GetName() : TEXT("NULL"));
		return;
	}

	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UObjectPoolSubSystem::Prewarm - Count가 0 이하입니다."));
		return;
	}

	FObjectPoolBucket& Bucket = Pools.FindOrAdd(ActorClass.Get());

	while (Bucket.ReadyObjects.Num() + Bucket.UsingObjects.Num() < Count)
	{
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(
			ActorClass,
			FTransform::Identity
		);

		if (!NewActor)
		{
			break;
		}

		IPoolableInterface::Execute_OnReturnToPool(NewActor);

		Bucket.ReadyObjects.Add(NewActor);
		UE_LOG(LogTemp, Log, TEXT("UObjectPoolSubSystem::Prewarm - %s 를 생성했습니다."), *NewActor->GetName());
	}
}

bool UObjectPoolSubSystem::IsPoolableClass(TSubclassOf<AActor> ActorClass) const
{
	return ActorClass && ActorClass->ImplementsInterface(UPoolableInterface::StaticClass());
}
