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

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Static Mesh"));
	ItemStaticMesh->SetupAttachment(SceneRoot);

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Skeletal Mesh"));
	ItemSkeletalMesh->SetupAttachment(SceneRoot);

	SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Component"));
	NiagaraComponent->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	InitialMeshLocation = ItemStaticMesh->GetComponentLocation();
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

	if (ItemStaticMesh)
	{
		ItemStaticMesh->AddRelativeRotation(FRotator(0.0f, 30.0f * DeltaTime, 0.0f));
		ItemStaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 5 * FMath::Cos(ElapsedTime * 0.7)));
	}

	if (ItemSkeletalMesh)
	{
		ItemSkeletalMesh->AddRelativeRotation(FRotator(0.0f, 30.0f * DeltaTime, 0.0f));
		ItemSkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 5 * FMath::Cos(ElapsedTime * 0.7)));
	}
}

void AItemBase::MoveVFXtoStaticMesh()
{
	NiagaraComponent->SetWorldLocation(ItemStaticMesh->GetComponentLocation());
}

void AItemBase::MoveVFXtoSkeletalMesh()
{
	NiagaraComponent->SetWorldLocation(ItemSkeletalMesh->GetComponentLocation());
}

