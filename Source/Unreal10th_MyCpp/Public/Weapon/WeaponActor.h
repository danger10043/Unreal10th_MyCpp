// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/WeaponUserInterface.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;
class UWeaponDataAsset;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class UNREAL10TH_MYCPP_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponActor();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void EquipToTarget(AActor* InOwner);

	UFUNCTION(BlueprintCallable)
	void DropWeapon();

	UFUNCTION(BlueprintCallable)
	void OnHitAreaBeginOverlap(
		UPrimitiveComponent* InOnComponentBeginOverlap,
		AActor* InOtherActor,
		UPrimitiveComponent* InOtherComp,
		int32 InOtherBodyIndex,
		bool bFromSweep,
		const FHitResult& InSweepResult
	);

	UFUNCTION(BlueprintCallable)
	void InitializeWeapon(UWeaponDataAsset* InData);

	UFUNCTION(BlueprintCallable)
	void AttackEnable(bool bEnable);

	UWeaponDataAsset* GetWeaponData() const;

	int32 GetAttackCount() const;

	void SetAttackCount(int32 InCount);

	void IncreaseCount();

	void DecreaseCount();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> HitArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UWeaponDataAsset> WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraComponent> TrailVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> HitVFX = nullptr;

	// 사용 가능한 무기 횟수
	int32 AttackCount = 0;
	
	// 한 번의 공격에서 공격한 액터들의 목록(공격 이후 초기화)
	TSet<AActor*> HitActors;

private:
	// 무기를 장비하고 있는 캐릭터
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

	// PhysicsDelay용 타이머 핸들
	FTimerHandle PhysicsDelayTimerHandle;
};
