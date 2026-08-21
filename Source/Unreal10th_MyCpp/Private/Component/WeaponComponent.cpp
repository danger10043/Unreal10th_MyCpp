// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WeaponComponent.h"
#include "Weapon/WeaponActor.h"
#include "Data/WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (BasicWeaponData)
	{
		EquipWeapon(BasicWeaponData);
	}
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::EquipWeapon(UWeaponDataAsset* InWeaponData)
{
    if (!InWeaponData) return;

    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->DropWeapon();
        CurrentWeapon = nullptr;
    }

    CurrentWeaponData = InWeaponData;

    if (!InWeaponData->IsLoadCompleted())
    {
        UWeaponDataAsset* RequestedData = InWeaponData;
        InWeaponData->RequestDataLoad(
            FStreamableDelegate::CreateWeakLambda(this, [this, RequestedData]()
                {
                    if (CurrentWeaponData == RequestedData)
                    {
                        SpawnWeaponActor();
                    }
                }));
        return;
    }

    SpawnWeaponActor();
}

void UWeaponComponent::SpawnWeaponActor()
{
    if (GetOwner())
    {
        UE_LOG(LogTemp, Log, TEXT("%s 가 WeaponActor 를 Spawn 을 시도했습니다."), *GetOwner()->GetName());
    }
    if (!CurrentWeaponData || !GetWorld() || !GetOwner())
    {
        if (!CurrentWeaponData)
        {
            UE_LOG(LogTemp, Warning, TEXT("경고 - CurrentWeaponData 가 NULL 입니다. WeaponActor Spawn 실패."));
		}
        else if (!GetWorld())
        {
            UE_LOG(LogTemp, Warning, TEXT("경고 - GetWorld 가 NULL 입니다. WeaponActor Spawn 실패."));
        }
        else if (!GetOwner())
        {
            UE_LOG(LogTemp, Warning, TEXT("경고 - GetOwner 가 NULL 입니다. WeaponActor Spawn 실패."));
		}
        return;
    }

    APawn* InstigatorPawn = Cast<APawn>(GetOwner());

    CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(
        AWeaponActor::StaticClass(),
        FTransform::Identity,
        GetOwner(),
        InstigatorPawn);

    if (!CurrentWeapon.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponActor Spawn 실패"));
        return;
    }
    CurrentWeapon->InitializeWeapon(CurrentWeaponData);
    UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);
    CurrentWeapon->EquipToTarget(GetOwner());
}

UAnimMontage* UWeaponComponent::GetAttackMontage() const
{
	return CurrentWeaponData ? CurrentWeaponData->AttackMontage.Get() : nullptr;
}

bool UWeaponComponent::ConsumeWeaponUse()
{
	if (!CurrentWeapon.IsValid())
	{
		return false;
	}

	if (CurrentWeapon->GetAttackCount() > 0)
	{
		CurrentWeapon->DecreaseCount();
		if (CurrentWeapon->GetAttackCount() == 0 && BasicWeaponData && CurrentWeaponData != BasicWeaponData)
		{
			EquipWeapon(BasicWeaponData);
		}
	}

	return true;
}
