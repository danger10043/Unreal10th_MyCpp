// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class USphereComponent;
class USceneComponent;
class UNiagaraComponent;
class UItemDataAsset;
class UNiagaraSystem;

UCLASS()
class UNREAL10TH_MYCPP_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

	virtual void InitializeFromItemData(UItemDataAsset* InItemData);

	void PrePareForDropSpawn();

	// Factory 가 아이템 생성 직후 호출
	void LaunchAsDroppedItem(
		const bool bDoLaunchToForwardOfPlayer = false,
		const FVector LaunchDirection = FVector(0, 0, 0)
	);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void PickUpItem(AActor* InActor);

	virtual void pickDownItem(AActor* InActor);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UItemDataAsset> ItemData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> DropPhysicsCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemStaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ItemSkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|VFX")
	TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Item|VFX")
	TObjectPtr<UNiagaraSystem> PickupNiagaraSystem = nullptr;

	// 설정된 획득 Niagara System을 월드에 스폰
	void SpawnPickupNiagaraEffect(AActor* InActor);

	// 수평 방향 발사 속도 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Drop Physics", meta = (ClampMin = "0.0"))
	FVector2D DropHorizontalSpeedRange = FVector2D(300.0f, 500.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Drop Physics", meta = (ClampMin = "0.0"))
	FVector2D DropVerticalSpeedRange = FVector2D(400.0f, 600.0f);

	// 바닥에 정착했는지 여부
	bool bHasSettled = true;

	bool bPickupDelayElapsed = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Drop Physics", meta = (ClampMin = "0.0"))
	float DropPickupDelay = 1.0f;

	FTimerHandle PickupDelayTimerHandle;

	void EnablePickupCollisionIfReady();

	UFUNCTION()
	void OnDropPhysicsHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	void FinishDropMovement();

	FVector InitialMeshLocation = FVector::Zero();

	float ElapsedTime = 0.0f;
	
	UFUNCTION(BlueprintCallable)
	void MoveVFXtoStaticMesh();

	UFUNCTION(BlueprintCallable)
	void MoveVFXtoSkeletalMesh();
};
