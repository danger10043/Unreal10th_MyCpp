// Fill out your copyright notice in the Description page of Project Settings.


#include "P0723_HealPickup.h"
#include "L0723_Player.h"
#include "L0723_StatComponent.h"
#include "P0723_StatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AP0723_HealPickup::AP0723_HealPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.0f);
	SphereCollision->SetGenerateOverlapEvents(true);
	SetRootComponent(SphereCollision);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SphereCollision);
}

// Called when the game starts or when spawned
void AP0723_HealPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AP0723_HealPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AP0723_HealPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (IP0723_StatComponent* Player = Cast<IP0723_StatComponent>(OtherActor))
	{
		UWorld* World = GetWorld();
		FTimerManager& TimerManager = World->GetTimerManager();

		TimerManager.SetTimer(
			HealthTickTimer,
			FTimerDelegate::CreateLambda(
				[this, Player]()
				{
					if (Health > 0) {
						Player->GetStatComponent()->IP0723_HealthInterface::Execute_HealHealth(Player->GetStatComponent(), Health);
					}
					else {
						Player->GetStatComponent()->IP0723_HealthInterface::Execute_DamageHealth(Player->GetStatComponent(), -Health);
						
					}
				}
			),
			HealthTickInterval,
			true
		);
	}
}

void AP0723_HealPickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	UWorld* World = GetWorld();
	FTimerManager& TimerManager = World->GetTimerManager();
	TimerManager.ClearTimer(HealthTickTimer);
}

