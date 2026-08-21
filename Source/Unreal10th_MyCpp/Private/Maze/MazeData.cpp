// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeData.h"

const FIntPoint FMazeData::Direction[4] =
{
	FIntPoint(0, 1), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(-1, 0)
};

FMazeData::~FMazeData()
{
	ClearMaze(); // 안전장치
}

void FMazeData::MakeMaze(uint8 InWidth, uint8 InHeight, EMazeAlgorithm InAlgorithm, int32 InSeed)
{
	ClearMaze();	// 이전에 만들어진 데이터 삭제하기

	Width = InWidth;
	Height = InHeight;

	if (Width == 0 || Height == 0) return;

	if (InSeed == RandomSeed)
	{
		// 랜덤하게 시드값 설정
		RandomStream.GenerateNewSeed();
	}
	else
	{
		// 정해진 값으로 시드값 설정
		RandomStream.Initialize(InSeed);
	}

	Cells.SetNum(Width * Height);	// 배열 초기화(SetNum 으로 실제 배열 요소도 생성)

	switch (InAlgorithm)
	{
	case EMazeAlgorithm::Wilson:
		WilsonAlgorithmExecute();
		break;
	case EMazeAlgorithm::Backtracking:
		BacktrackingAlgorithmExecute();
		break;
	case EMazeAlgorithm::Eller:
		UE_LOG(LogTemp, Warning, TEXT("FMazeData::MakeMaze - Eller 알고리즘은 아직 구현되지 않았습니다."));
		ClearMaze();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("FMazeData::MakeMaze - 지원하지 안흔 알고리즘입니다."));
		ClearMaze();
		break;
	}
}

void FMazeData::ClearMaze()
{
	Width = 0;
	Height = 0;
	Cells.Empty();
}

void FMazeData::WilsonAlgorithmExecute()
{
	// 기본 초기화
	TArray<FWilsonCellState> CellStates;
	CellStates.SetNum(Cells.Num());

	TArray<FCellData*> NotInMazeCells;
	NotInMazeCells.Reserve(Cells.Num());

	for (int32 y = 0; y < static_cast<int32>(Height); ++y)
	{
		for (int32 x = 0; x < static_cast<int32>(Width); ++x)
		{
			FCellData* Cell = GetCell(x, y);
			if (!Cell)
			{
				continue;
			}
			Cell->X = static_cast<uint8>(x);
			Cell->Y = static_cast<uint8>(y);

			NotInMazeCells.Add(Cell);
		}
	}

	ShuffleArray(NotInMazeCells);

	if (NotInMazeCells.IsEmpty())
	{
		return;
	}

	// Wilson 알고리즘 시작
	FCellData* InitialCell = NotInMazeCells.Pop();
	const int32 InitialCellIndex = LocationToIndex(InitialCell->X, InitialCell->Y);

	CellStates[InitialCellIndex].bInMaze = true;

	while (!NotInMazeCells.IsEmpty())
	{
		FCellData* StartCell = NotInMazeCells.Pop();
		const int32 StartCellIndex = LocationToIndex(StartCell->X, StartCell->Y);

		if (CellStates[StartCellIndex].bInMaze)
		{
			continue;
		}

		FCellData* CurrentCell = StartCell;
		int32 CurrentCellIndex = StartCellIndex;

		do
		{
			FCellData* NeighborCell = GetRandomNeighborCell(*CurrentCell);

			if (!NeighborCell) return;

			const int32 NeighborCellIndex = LocationToIndex(NeighborCell->X, NeighborCell->Y);

			CellStates[CurrentCellIndex].NextCellIndex = NeighborCellIndex;

			CurrentCell = NeighborCell;
			CurrentCellIndex = NeighborCellIndex;
		} while (!CellStates[CurrentCellIndex].bInMaze);

		int32 PathCellIndex = StartCellIndex;

		while (PathCellIndex != CurrentCellIndex)
		{
			const int32 NextCellIndex = CellStates[PathCellIndex].NextCellIndex;

			if (!Cells.IsValidIndex(NextCellIndex))
			{
				UE_LOG(
					LogTemp,
					Error,
					TEXT("FMazeData::WilsonAlgorithmExecute - Wilson 경로 인덱스가 유효하지 않습니다.")
				);
				return;
			}

			CellStates[PathCellIndex].bInMaze = true;

			ConnectCells(&Cells[PathCellIndex], &Cells[NextCellIndex]);

			PathCellIndex = NextCellIndex;
		}
	}
}

void FMazeData::BacktrackingAlgorithmExecute()
{
	if (Cells.IsEmpty())
	{
		return;
	}

	TArray<FBacktrackingCellState> CellStates;
	CellStates.SetNum(Cells.Num());

	for (int32 y = 0; y < static_cast<int32>(Height); ++y)
	{
		for (int32 x = 0; x < static_cast<int32>(Width); ++x)
		{
			FCellData* Cell = GetCell(x, y);
			if (!Cell)
			{
				continue;
			}

			Cell->X = static_cast<uint8>(x);
			Cell->Y = static_cast<uint8>(y);
		}
	}

	const int32 StartCellIndex = RandomStream.RandRange(0, Cells.Num() - 1);

	RecursiveBacktracking(StartCellIndex, CellStates);
}

void FMazeData::RecursiveBacktracking(
	int32 InCurrentCellIndex, 
	TArray<FBacktrackingCellState>& InOutCellStates)
{
	if (!Cells.IsValidIndex(InCurrentCellIndex) || !InOutCellStates.IsValidIndex(InCurrentCellIndex))
	{
		return;
	}

	InOutCellStates[InCurrentCellIndex].bVisited = true;

	const FCellData& CurrentCell = Cells[InCurrentCellIndex];

	TArray<int32> NeighborCellIndices;
	NeighborCellIndices.Reserve(DirectionCount);

	for (int32 DirectionIndex = 0; DirectionIndex < DirectionCount; ++DirectionIndex)
	{
		const FIntPoint NeighborLocation = CurrentCell.GetLocation() + Direction[DirectionIndex];

		if (!IsValidLocation(NeighborLocation.X, NeighborLocation.Y))
		{
			continue;
		}

		const int32 NeighborCellIndex = LocationToIndex(NeighborLocation.X, NeighborLocation.Y);

		if (!InOutCellStates[NeighborCellIndex].bVisited)
		{
			NeighborCellIndices.Add(NeighborCellIndex);
		}
	}

	for (int32 i = NeighborCellIndices.Num() - 1; i > 0; --i)
	{
		const int32 RandomIndex = RandomStream.RandRange(0, i);

		NeighborCellIndices.Swap(i, RandomIndex);
	}

	for (const int32 NeighborCellIndex : NeighborCellIndices)
	{
		if (InOutCellStates[NeighborCellIndex].bVisited)
		{
			continue;
		}

		InOutCellStates[NeighborCellIndex].PreviousCellIndex = InCurrentCellIndex;

		ConnectCells(&Cells[InCurrentCellIndex], &Cells[NeighborCellIndex]);
		RecursiveBacktracking(NeighborCellIndex, InOutCellStates);
	}
}

void FMazeData::EllerAlgorithmExecute()
{
}

FCellData* FMazeData::GetCell(int32 InX, int32 InY)
{
	if (!IsValidLocation(InX, InY)) return nullptr;

	return &(Cells[LocationToIndex(InX, InY)]);
}

void FMazeData::ConnectCells(FCellData* InFrom, FCellData* InTo)
{
	// From과 To가 모두 있어야 하고, 서로 다른 셀이어야 한다.
	if (!InFrom || !InTo || (InFrom == InTo)) return;

	const int32 DeltaX =
		static_cast<int32>(InTo->X) - static_cast<int32>(InFrom->X);
	const int32 DeltaY =
		static_cast<int32>(InTo->Y) - static_cast<int32>(InFrom->Y);

	if (FMath::Abs(DeltaX) + FMath::Abs(DeltaY) != 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("FMazeData::ConnectCells - ConnectCells에는 서로 인접한 셀을 전달해야 합니다."));
		return;
	}


	if (InFrom->X < InTo->X)
	{
		// From의 오른쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::Right);
		InTo->AddPath(EDirectionType::Left);
	}
	else if (InFrom->X > InTo->X)
	{
		// From의 왼쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::Left);
		InTo->AddPath(EDirectionType::Right);
	}
	else if (InFrom->Y < InTo->Y)
	{
		// From의 아래쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::Down);
		InTo->AddPath(EDirectionType::Up);
	}
	else
	{
		// From의 위쪽에 To가 있다.
		InFrom->AddPath(EDirectionType::Up);
		InTo->AddPath(EDirectionType::Down);
	}
}

FCellData* FMazeData::GetRandomNeighborCell(const FCellData& InCell)
{
	FIntPoint NeighborLocation;

	do
	{
		int32 Index = RandomStream.RandRange(0, DirectionCount - 1); // 0, 1, 2, 3 중 하나
		NeighborLocation = InCell.GetLocation() + Direction[Index];
	} while (!IsValidLocation(NeighborLocation.X, NeighborLocation.Y)); // 미로 밖을 선택하는 일을 방지

	return GetCell(NeighborLocation.X, NeighborLocation.Y);
}

void FMazeData::ShuffleArray(TArray<FCellData*>& InOutArray)
{
	for (int i = InOutArray.Num() - 1; i > 0; i--)
	{
		int32 Index = RandomStream.RandRange(0, i);
		InOutArray.Swap(i, Index);
	}
}
