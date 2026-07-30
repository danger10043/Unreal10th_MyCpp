// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"
#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene"));
	SetRootComponent(SceneRoot);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->InitSphereRadius(100.0f);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetupAttachment(SceneRoot);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetupAttachment(SceneRoot);

	SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Component"));
	NiagaraComponent->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	InitialItemMeshLocation = ItemMesh->GetComponentLocation();
}

void AItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	PickUpItem(OtherActor);
}

void AItemBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	pickDownItem(OtherActor);

	Super::NotifyActorEndOverlap(OtherActor);
}

void AItemBase::PickUpItem(AActor* InActor)
{
}

void AItemBase::pickDownItem(AActor* InActor)
{
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElapsedTime += DeltaTime;
	ItemMesh->AddRelativeRotation(FRotator(0.0f, 30.0f * DeltaTime, 0.0f));
	ItemMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 20 * FMath::Cos(ElapsedTime * 2)));
}

