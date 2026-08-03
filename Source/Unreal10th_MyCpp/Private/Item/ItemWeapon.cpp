// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemWeapon.h"
#include "Components/StaticMeshComponent.h"

#include "Player/ActionPlayer.h"
#include "Component/StatComponent.h"
#include "Interface/StatComponentInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "Weapon/WeaponActor.h"
#include "Components/SphereComponent.h"
#include "Data/WeaponDataAsset.h"

// Sets default values
AItemWeapon::AItemWeapon()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AItemWeapon::BeginPlay()
{
    Super::BeginPlay();
    ItemSkeletalMesh->SetRelativeRotation(FRotator(45.0f, 0.0f, 0.0f));
}

void AItemWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItemWeapon::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (WeaponData)
    {
        USkeletalMesh* SkeletalMeshData = WeaponData->Mesh.LoadSynchronous();
        {
            ItemSkeletalMesh->SetSkeletalMesh(SkeletalMeshData);
        }
    }
}

void AItemWeapon::PickUpItem(AActor* InActor)
{
    if (GetWorldTimerManager().IsTimerActive(PickupEffectTimerHandle)) return;

    if (!InActor->GetClass()->ImplementsInterface(UWeaponUserInterface::StaticClass())) return;

    Super::PickUpItem(InActor);

    SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PickupStartLocation = ItemSkeletalMesh->GetComponentLocation();
    PickupElapsedTime = 0.0f;

    GetWorldTimerManager().SetTimer(
        PickupEffectTimerHandle,
        FTimerDelegate::CreateWeakLambda(
            this,
            [this, InActor]()
            {
                if (!LocationCurve || !ScaleCurve)
                {
                    GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);
                    return;
                }

                PickupElapsedTime += 0.01f;
                float Progress = PickupElapsedTime / 1.5f;

                float DistanceAlpha = LocationCurve->GetFloatValue(Progress);
                FVector Goal = InActor->GetActorLocation();
                FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);

                ItemSkeletalMesh->SetWorldLocation(NewLocation);
                MoveVFXtoSkeletalMesh();

                float ScaleAlpha = ScaleCurve->GetFloatValue(Progress);
                float NewScale = FMath::Lerp(1.0f, 0.0f, ScaleAlpha);
                ItemSkeletalMesh->SetRelativeScale3D(FVector(NewScale));

                ItemSkeletalMesh->SetRelativeRotation(FRotator(45.0f, PickupElapsedTime * 1000, 0.0f));

                if (Progress >= 1.0f)
                {
                    GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);

                    if (InActor && InActor->GetClass()->ImplementsInterface(UWeaponUserInterface::StaticClass()))
                    {
                        IWeaponUserInterface::Execute_EquipWeapon(InActor, WeaponData);
                    }

                    Destroy();
                }
            }
        ),
        0.01f,
        true
    );
}

