// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeActor.h"
#include "Maze/CellActor.h"
#include "Maze/CellData.h"
#include "Components/SceneComponent.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMazeActor::AMazeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);
	
	FloorHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloorHISM"));
	FloorHISM->SetupAttachment(SceneRoot);
	FloorHISM->SetCollisionProfileName(TEXT("BlockAll"));

	WallHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallHISM"));
	WallHISM->SetupAttachment(SceneRoot);
	WallHISM->SetCollisionProfileName(TEXT("BlockAll"));

	GateHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("GateHISM"));
	GateHISM->SetupAttachment(SceneRoot);
	GateHISM->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	CellActorClass = ACellActor::StaticClass();
}

// Called when the game starts or when spawned
void AMazeActor::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateMaze();
}

void AMazeActor::GenerateMaze()
{
	ClearMazeActors();

	if (!CellActorClass && !bUseHISM)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMazeActor::GenerateMaze - CellActorClass가 설정되지 않았습니다."));
		return;
	}

	if (MazeWidth <= 0 || MazeHeight <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMazeActor::GenerateMaze - 미로의 크기는 1 이상이어야 합니다."));
		return;
	}

	if (bUseHISM)
	{
		BuildMazeHISM(&MazeData);
	}
	else
	{
		SpawnCells(&MazeData);
	}
}

void AMazeActor::SpawnCells(FMazeData* InMaze)
{
	if (!InMaze)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMazeActor::SpawnCells - MazeData가 유효하지 않습니다."));
		return;
	}

	const uint8 ValidWidth = static_cast<uint8>(FMath::Clamp(MazeWidth, 1, 255));
	const uint8 ValidHeight = static_cast<uint8>(FMath::Clamp(MazeHeight, 1, 255));

	InMaze->MakeMaze(ValidWidth, ValidHeight, MazeAlgorithm, MazeSeed);

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMazeActor::SpawnCells - World가 유효하지 않습니다."));
		return;
	}

	const TArray<FCellData>& Cells = InMaze->GetCells();
	SpawnedCellActors.Reserve(Cells.Num());

	const float HalfMazeHeight = (static_cast<float>(InMaze->GetHeight()) - 1.0f) * CellSpacing * 0.5f;
	const float HalfMazeWidth = (static_cast<float>(InMaze->GetWidth()) - 1.0f) * CellSpacing * 0.5f;

	const FVector StartLocation(HalfMazeHeight, -HalfMazeWidth, 0.0f);

	for (const FCellData& CellData : Cells)
	{
		const FVector LocalLocation = StartLocation + FVector(
			-static_cast<float>(CellData.Y) * CellSpacing,
			static_cast<float>(CellData.X) * CellSpacing,
			0.0f
		);

		const FVector WorldLocation = GetActorTransform().TransformPosition(LocalLocation);

		const FTransform SpawnTransform(
			GetActorRotation(),
			WorldLocation
		);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ACellActor* SpawnedCell =
			World->SpawnActor<ACellActor>(
				CellActorClass,
				SpawnTransform,
				SpawnParameters
			);

		if (!SpawnedCell)
		{
			UE_LOG(LogTemp, Warning, TEXT("AMazeActor::SpawnCells - CellActor 생성에 실패했습니다. 좌표 : (%d, %d)"),
				static_cast<int32>(CellData.X),
				static_cast<int32>(CellData.Y));
			continue;
		}

		SpawnedCell->AttachToActor(
			this,
			FAttachmentTransformRules::KeepWorldTransform
		);

		SpawnedCell->InitializeCell(&CellData);
		SpawnedCellActors.Add(SpawnedCell);
	}
}

void AMazeActor::BuildMazeHISM(FMazeData* InMaze)
{
	if (!InMaze)
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor::BuildMazeHISM - MazeData가 유효하지 않습니다."));
		return;
	}

	const uint8 ValidWidth = static_cast<uint8>(FMath::Clamp(MazeWidth, 1, 255));
	const uint8 ValidHeight = static_cast<uint8>(FMath::Clamp(MazeHeight, 1, 255));

	InMaze->MakeMaze(ValidWidth, ValidHeight, MazeAlgorithm, MazeSeed);

	if (!FloorHISM || !FloorHISM->GetStaticMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor::BuildMazeHISM - FloorHISM의 Mesh가 유효하지 않습니다."));
		return;
	}

	if (!WallHISM || !WallHISM->GetStaticMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor::BuildMazeHISM - WallHISM의 Mesh가 유효하지 않습니다."));
		return;
	}

	if (!GateHISM || !GateHISM->GetStaticMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor::BuildMazeHISM - GateHISM의 Mesh가 유효하지 않습니다."));
		return;
	}

	ClearHISMInstances();

	const float HalfMazeHeight = (static_cast<float>(InMaze->GetHeight()) - 1.0f) * CellSpacing * 0.5f;
	const float HalfMazeWidth = (static_cast<float>(InMaze->GetWidth()) - 1.0f) * CellSpacing * 0.5f;

	const FVector StartLocation(HalfMazeHeight, -HalfMazeWidth, 0.0f);

	for (int32 y = 0; y < static_cast<int32>(InMaze->GetHeight()); ++y) 
	{
		for (int32 x = 0; x < static_cast<int32>(InMaze->GetWidth()); ++x)
		{
			FCellData* Cell = InMaze->GetCell(x, y);
			if (!Cell) continue;

			const FVector CellLocation = StartLocation + FVector(-y * CellSpacing, x * CellSpacing, 0.0f);
			MakeCellHISM(Cell, CellLocation);
		}
	}
}

void AMazeActor::ClearHISMInstances()
{
	if (FloorHISM)
	{
		FloorHISM->ClearInstances();
	}
	if (WallHISM)
	{
		WallHISM->ClearInstances();
	}
	if (GateHISM)
	{
		GateHISM->ClearInstances();
	}
	FlushPersistentDebugLines(GetWorld());
}

void AMazeActor::MakeCellHISM(const FCellData* InCell, const FVector& InLocation)
{
	if (!InCell)
	{
		UE_LOG(LogTemp, Warning, TEXT("MazeActor::MakeCellHISM - CellData가 유효하지 않습니다."));
		return;
	}

	// 바닥 추가
	FloorHISM->AddInstance(FTransform(FRotator::ZeroRotator, InLocation));

	// 벽 추가
	FVector UpWallLocation = InLocation + FVector::ForwardVector * (CellSpacing / 2);
	WallHISM->AddInstance(FTransform(FRotator::ZeroRotator, UpWallLocation));
	if (InCell->IsWall(EDirectionType::Up))
	{
		GateHISM->AddInstance(FTransform(FRotator::ZeroRotator, UpWallLocation));
	}

	FVector LeftWallLocation = InLocation + FVector::LeftVector * (CellSpacing / 2);
	WallHISM->AddInstance(FTransform(FRotator(0.0f, -90.0f, 0.0f), LeftWallLocation));
	if (InCell->IsWall(EDirectionType::Left))
	{
		GateHISM->AddInstance(FTransform(FRotator(0.0f, -90.0f, 0.0f), LeftWallLocation));
	}

	if (InCell->X == MazeData.GetWidth() - 1)
	{
		FVector RightWallLocation = InLocation + FVector::RightVector * (CellSpacing / 2);
		WallHISM->AddInstance(FTransform(FRotator(0.0f, 90.0f, 0.0f), RightWallLocation));
		GateHISM->AddInstance(FTransform(FRotator(0.0f, 90.0f, 0.0f), RightWallLocation));
	}

	if (InCell->Y == MazeData.GetHeight() - 1)
	{
		FVector DownWallLocation = InLocation + FVector::BackwardVector * (CellSpacing / 2);
		WallHISM->AddInstance(FTransform(FRotator(0.0f, 180.0f, 0.0f), DownWallLocation));
		GateHISM->AddInstance(FTransform(FRotator(0.0f, 180.0f, 0.0f), DownWallLocation));
	}

#if WITH_EDITOR
	const FVector DebugWorldLocation = FloorHISM->GetComponentTransform().TransformPosition(InLocation);

	DrawDebugBox(
		GetWorld(),
		DebugWorldLocation,
		FVector(CellSpacing / 2, CellSpacing / 2, 400.0f),
		FColor::Green,
		true,
		-1.0f,
		0,
		3.0f
	);
#endif
}

void AMazeActor::ClearMazeActors()
{
	ClearHISMInstances();

	for (ACellActor* CellActor : SpawnedCellActors)
	{
		if (IsValid(CellActor))
		{
			CellActor->Destroy();
		}
	}

	SpawnedCellActors.Empty();
	MazeData.ClearMaze();
}

void AMazeActor::OnPreMazeGenerate()
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem)
	{
		NavSystem->SetNavigationAutoUpdateEnabled(false, NavSystem);	// NavMesh 자동 업데이트 막기
	}
}

void AMazeActor::OnPostMazeGenerate()
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem)
	{
		NavSystem->Build();	// NavMesh 수동 생성
		NavSystem->SetNavigationAutoUpdateEnabled(true, NavSystem);	// NavMesh 자동 업데이트 재개
	}
}

void AMazeActor::TestMakeMaze()
{
	UE_LOG(LogTemp, Log, TEXT("AMazeActor::TestMakeMaze - 실행됨."));
	GenerateMaze();
}

void AMazeActor::TestClearMaze()
{
	UE_LOG(LogTemp, Log, TEXT("AMazeActor::TestClearMaze - 실행됨."));
	ClearMazeActors();
}
