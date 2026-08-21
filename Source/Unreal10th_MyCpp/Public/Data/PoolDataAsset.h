// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PoolDataAsset.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EPoolAcquireOption : uint8
{
	DoNotSpawn	UMETA(DisplayName = "더 이상 생성하지 않음"),
	Grow		UMETA(DisplayName = "계속 생성 (MaxSize 무시)"),
	ReuseOldest	UMETA(DisplayName = "가장 오래된 액터 재사용")
};

UCLASS(BlueprintType)
class UNREAL10TH_MYCPP_API UPoolDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> PooledActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPoolAcquireOption AcquireOption = EPoolAcquireOption::Grow;

	// 게임 시작 시 미리 새성할 수량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 InitialPoolSize = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 MaxPoolSize = 30;
};
