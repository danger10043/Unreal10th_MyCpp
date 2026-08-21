// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/WeaponDataAsset.h"
#include "Engine/AssetManager.h"
#include "Data/PoolDataAsset.h"
#include "NiagaraSystem.h"

void UWeaponDataAsset::RequestDataLoad(FStreamableDelegate InDelegate)
{
    TArray<FSoftObjectPath> TargetsToLoad;

    if (!Mesh.IsNull())
    {
        TargetsToLoad.Add(Mesh.ToSoftObjectPath());
    }

    if (!TrailVFX.IsNull())
    {
        TargetsToLoad.Add(TrailVFX.ToSoftObjectPath());
    }

    if (!HitVFX.IsNull())
    {
        TargetsToLoad.Add(HitVFX.ToSoftObjectPath());
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    AsyncLoadHandle = Streamable.RequestAsyncLoad(
        TargetsToLoad,
        MoveTemp(InDelegate)
    );
}

bool UWeaponDataAsset::IsLoadCompleted() const
{
	return AsyncLoadHandle.IsValid() && AsyncLoadHandle.Get()->HasLoadCompleted() && Mesh.IsValid()
        && TrailVFX.IsValid()
        && HitVFX.IsValid();
}
