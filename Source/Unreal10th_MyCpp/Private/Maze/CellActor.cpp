// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/CellActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
ACellActor::ACellActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	SetRootComponent(FloorMesh);
	FloorMesh->SetCollisionProfileName(TEXT("BlockAll"));

	UStaticMeshComponent* Wall;
	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall(Up)"));
	Wall->SetupAttachment(GetRootComponent());
	Wall->SetCollisionProfileName(TEXT("BlockAll"));
	Wall->SetRelativeLocationAndRotation(
		FVector::ForwardVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 0, 0)
	);
	WallMeshes.Add(Wall);

	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall(Right)"));
	Wall->SetupAttachment(GetRootComponent());
	Wall->SetCollisionProfileName(TEXT("BlockAll"));
	Wall->SetRelativeLocationAndRotation(
		FVector::RightVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 90, 0)
	);
	WallMeshes.Add(Wall);

	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall(Down)"));
	Wall->SetupAttachment(GetRootComponent());
	Wall->SetCollisionProfileName(TEXT("BlockAll"));
	Wall->SetRelativeLocationAndRotation(
		FVector::BackwardVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 180, 0)
	);
	WallMeshes.Add(Wall);


	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall(Left)"));
	Wall->SetupAttachment(GetRootComponent());
	Wall->SetCollisionProfileName(TEXT("BlockAll"));
	Wall->SetRelativeLocationAndRotation(
		FVector::LeftVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 270, 0)
	);
	WallMeshes.Add(Wall);


	UStaticMeshComponent* Gate;
	Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate(Up)"));
	Gate->SetupAttachment(GetRootComponent());
	Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Gate->SetRelativeLocationAndRotation(
		FVector::ForwardVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 0, 0)
	);
	GateMeshes.Add(Gate);

	Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate(Right)"));
	Gate->SetupAttachment(GetRootComponent());
	Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Gate->SetRelativeLocationAndRotation(
		FVector::RightVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 90, 0)
	);
	GateMeshes.Add(Gate);

	Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate(Down)"));
	Gate->SetupAttachment(GetRootComponent());
	Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Gate->SetRelativeLocationAndRotation(
		FVector::BackwardVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 180, 0)
	);
	GateMeshes.Add(Gate);

	Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate(Left)"));
	Gate->SetupAttachment(GetRootComponent());
	Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Gate->SetRelativeLocationAndRotation(
		FVector::LeftVector * (CellHalfSize - WallHalfThickness),
		FRotator(0, 270, 0)
	);
	GateMeshes.Add(Gate);

	UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow(Up)"));
	Arrow->SetupAttachment(GetRootComponent());
	Arrow->SetRelativeLocation(FVector(0, 0, 200));
	SetCanBeDamaged(false);
}

void ACellActor::InitializeCell(const FCellData* InCellData)
{
	if (!InCellData) return;

	Path = static_cast<int32>(InCellData->Path);
	OpenGate();
}

// Called when the game starts or when spawned
void ACellActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACellActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACellActor::TestPath()
{
	UE_LOG(LogTemp, Log, TEXT("CallActor::TestPath - 실행됨."));
	OpenGate();
}

void ACellActor::OpenGate()
{
	constexpr int DirectionCount = 4;
	for (int i = 0; i < DirectionCount; i++)
	{
		EDirectionType Dir = static_cast<EDirectionType>(1 << i);
		if (IsPath(Dir))
		{
			GateMeshes[i]->SetVisibility(false);
			GateMeshes[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			GateMeshes[i]->SetVisibility(true);
			GateMeshes[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

// CellActor.cpp 수정

bool ACellActor::IsPath(EDirectionType InDirection)
{
	const EDirectionType CurrentPath = static_cast<EDirectionType>(Path);

	return (CurrentPath & InDirection) == InDirection;
}

