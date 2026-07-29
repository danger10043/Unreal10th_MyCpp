// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene"));
	SetRootComponent(SceneRoot);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->InitSphereRadius(100.0f);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetupAttachment(SceneRoot);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetupAttachment(SceneRoot);

}

void AItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	PickUpItem(OtherActor);
}

void AItemBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	pickDownItem(OtherActor);
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

