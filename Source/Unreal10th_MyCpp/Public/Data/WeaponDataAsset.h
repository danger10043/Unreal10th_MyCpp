// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "WeaponDataAsset.generated.h"

/**
 *
 */
class UStaticMesh;
class UNiagaraSystem;
class UPoolDataAsset;

UCLASS()
class UNREAL10TH_MYCPP_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	void RequestDataLoad(FStreamableDelegate InDelegate);
	bool IsLoadCompleted() const;

public:
	// 무기의 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	// 무기의 Trail VFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<UNiagaraSystem> TrailVFX;

	// 무기의 Hit VFX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TSoftObjectPtr<UNiagaraSystem> HitVFX;

	// 무기의 Damage Popup Actor Pool Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<UPoolDataAsset> DamagePopupPoolData;

	// 무기 Damage Popup Actor의 글자 색상
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FLinearColor DamageTextColor = FLinearColor::White;

	// 무기 Damage Popup Actor의 글자 테두리 색상
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FLinearColor DamageTextOutlineColor = FLinearColor::Black;

	// 무기가 Attach 될 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FName AttachSocketName = TEXT("hand_rSocket");

	// HitArea의 Offset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	FVector HitAreaLocationOffset = FVector::Zero();

	// HitArea 캡슐의 높이 절반
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	float HitAreaHalfHeight = 60.0f;

	// HitArea 캡슐의 반지름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
	float HitAreaRadius = 30.0f;

	// 무기의 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FVector WeaponScale = FVector::OneVector;

	// 무기의 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	float AttackPower = 10.0f;

	// 무기의 사용 횟수(-1인 경우 무한 사용 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	int32 AttackCount = -1;

	// 무기가 드랍 된 후 사라질 때까지의 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DropLifeSpan = 10.0f;

	// 드랍 직후에 플레이어와 물리 상호작용이 안되는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float PhysicsDelay = 0.8f;

	// 이 무기의 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

protected:
	TSharedPtr<FStreamableHandle> AsyncLoadHandle;
};
