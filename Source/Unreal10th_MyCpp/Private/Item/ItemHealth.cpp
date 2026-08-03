// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemHealth.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Player/ActionPlayer.h"
#include "Component/StatComponent.h"
#include "Interface/StatComponentInterface.h"
#include "Interface/HealthInterface.h"

// Sets default values
AItemHealth::AItemHealth()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void AItemHealth::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItemHealth::BeginPlay()
{
    Super::BeginPlay();
    MeshMID = ItemStaticMesh->CreateDynamicMaterialInstance(0);
    
    if (!MeshMID)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s의 Dynamic Material Instance 생성에 실패했습니다."), *this->GetName());
    }
}

float AItemHealth::GetHealth() const
{
    return Health;
}

float AItemHealth::GetHealthTickInterval() const
{
    return HealthTickInterval;
}

void AItemHealth::SetHealth(float InHealth)
{
    Health = InHealth;
}

void AItemHealth::SetHealthTickInterval(float InHealthTickInterval)
{
    HealthTickInterval = InHealthTickInterval;
}

void AItemHealth::PickUpItem(AActor* InActor)
{
    if (IStatComponentInterface* Player = Cast<IStatComponentInterface>(InActor))
    {
        UWorld* World = GetWorld();
        FTimerManager& TimerManager = World->GetTimerManager();
        UStatComponent* StatComponent = Player->GetStatComponent();

        TimerManager.ClearTimer(ItemMeshBrightnessDecreaseTimerHandle);
        
        TimerManager.SetTimer(
            HealthTickTimer,
            FTimerDelegate::CreateLambda(
                [this, StatComponent]()
                {
                    if (Health > 0.0f) {
                        StatComponent->IHealthInterface::Execute_HealHealth(StatComponent, Health);
                    }
                    else {
                        StatComponent->IHealthInterface::Execute_DamageHealth(StatComponent, -Health);

                    }
                }
            ),
            HealthTickInterval,
            true
        );

        TimerManager.SetTimer(
            ItemMeshRotationTimerHandle,
            FTimerDelegate::CreateLambda(
                [&]()
                {
                    ItemStaticMesh->AddRelativeRotation(FRotator(0.0f, 4.5f, 0.0f));
                }
            ),
            0.01f,
            true
        );

        TimerManager.SetTimer(
            ItemMeshBrightnessIncreaseTimerHandle,
            FTimerDelegate::CreateLambda(
                [&]()
                {
                    CurrentMeshBright += MeshBrightChangeAmount;
                    if (MeshMID)
                    {
                        MeshMID->SetScalarParameterValue(MIDBrightnessName, CurrentMeshBright);
                    }

                    if (CurrentMeshBright >= MaxMeshBright)
                    {
                        TimerManager.ClearTimer(ItemMeshBrightnessIncreaseTimerHandle);
                    }
                }
            ),
            0.03f,
            true
        );
    }
}

void AItemHealth::pickDownItem(AActor* InActor)
{
    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();
    TimerManager.ClearTimer(HealthTickTimer);
    TimerManager.ClearTimer(ItemMeshBrightnessIncreaseTimerHandle);
    TimerManager.ClearTimer(ItemMeshRotationTimerHandle);

    TimerManager.SetTimer(
        ItemMeshBrightnessDecreaseTimerHandle,
        FTimerDelegate::CreateLambda(
            [&]()
            {
                CurrentMeshBright -= MeshBrightChangeAmount;
                if (MeshMID)
                {
                    MeshMID->SetScalarParameterValue(MIDBrightnessName, CurrentMeshBright);
                }

                if (CurrentMeshBright <= MinMeshBright)
                {
                    TimerManager.ClearTimer(ItemMeshBrightnessDecreaseTimerHandle);
                }
            }
        ),
        0.03f,
        true
    );
}




