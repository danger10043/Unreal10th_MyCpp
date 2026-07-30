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
        UStatComponent* StatComponent = Player->GetStatComponent()
            ;
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
    }
}

void AItemHealth::pickDownItem(AActor* InActor)
{
    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();
    TimerManager.ClearTimer(HealthTickTimer);
}




