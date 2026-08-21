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
#include "Data/ItemDataAsset.h"

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

void AItemWeapon::InitializeFromItemData(UItemDataAsset* InItemData)
{
    Super::InitializeFromItemData(InItemData);

    if (!IsValid(InItemData))
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemWeapon::InitializeFromItemData - ItemData 가 유효하지 않습니다."));
        return;
    }
    WeaponData = InItemData->WeaponData;
}

void AItemWeapon::PickUpItem(AActor* InActor)
{
    Super::PickUpItem(InActor);
}

