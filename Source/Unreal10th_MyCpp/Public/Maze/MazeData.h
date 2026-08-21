// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maze/CellData.h"

UENUM(BlueprintType)
enum class EMazeAlgorithm : uint8
{
	Backtracking	UMETA(DisplayName = "Backtracking"),
	Wilson			UMETA(DisplayName = "Wilson"),
	Eller			UMETA(DisplayName = "Eller")
};

// Wilson 알고리즘에서만 사용하는 임시 데이터
struct FWilsonCellState
{
	bool bInMaze = false;
	int32 NextCellIndex = INDEX_NONE;
};

// Backtracking 알고리즘에서만 사용하는 임시 데이터
struct FBacktrackingCellState
{
	bool bVisited = false;
	int32 PreviousCellIndex = INDEX_NONE;
};

// Eller 알고리즘에서만 사용하는 임시 데이터
struct FEllerCellState
{
	int32 SetId = INDEX_NONE;
};

class UNREAL10TH_MYCPP_API FMazeData
{
public:
	FMazeData() = default;
	~FMazeData(); // FMazeData가 상속이 된다면 반드시 가상 소멸자를 사용해야 한다.

	FMazeData(const FMazeData&) = delete;
	FMazeData& operator=(const FMazeData&) = delete;

	// 미로를 생성하는 함수
	void MakeMaze(
		uint8 InWidth, 
		uint8 InHeight, 
		EMazeAlgorithm InAlgorithm,
		int32 InSeed = RandomSeed
	);

	// 생성된 미로를 제거하는 함수
	void ClearMaze();

	const TArray<FCellData>& GetCells() const { return Cells; }
	
	uint8 GetWidth() const { return Width; }

	uint8 GetHeight() const { return Height; }

	// 특정 위치의 셀을 리턴하는 함수
	FCellData* GetCell(int32 InX, int32 InY);

private:
	// Wilson 알고리즘을 실행하는 함수
	void WilsonAlgorithmExecute();

	// 재귀 백트래킹 알고리즘을 실행하는 함수
	void BacktrackingAlgorithmExecute();

	void RecursiveBacktracking(int32 InCurrentCellIndex, TArray<FBacktrackingCellState>& InOutCellStates);

	// Eller 알고리즘
	void EllerAlgorithmExecute();


	// From셀과 To셀 사이의 문을 제거하는 함수
	void ConnectCells(FCellData* InFrom, FCellData* InTo);

	// InCell 주변의 셀 중 하나를 랜덤으로 리턴하는 함수
	FCellData* GetRandomNeighborCell(const FCellData& InCell);

	// 배열의 순서를 섞는 함수
	void ShuffleArray(TArray<FCellData*>& InOutArray);

	// 위치를 인덱스로 변경하는 함수
	inline uint16 LocationToIndex(int32 InX, int32 InY) const { return InX + InY * Width; }

	// 위치가 미로 범위 안인지 확인하는 함수
	inline bool IsValidLocation(int32 InX, int32 InY) const {
		return	InX < static_cast<int32>(Width) &&
			InY < static_cast<int32>(Height) &&
			InX >= 0 &&
			InY >= 0;
		}

private:
	static constexpr int32 RandomSeed = -1;

	// 미로의 가로 칸(Cell) 수
	uint8 Width = 0;
	// 미로의 세로 칸(Cell) 수
	uint8 Height = 0;

	// 미로의 모든 칸(Cell)이 들어있는 배열
	TArray<FCellData> Cells;

	// 랜덤한 수를 뽑기 위한 스트림
	FRandomStream RandomStream;

	// 이웃 방향의 개수(4방향)
	static constexpr int DirectionCount = 4;
	// 이웃 방향의 상대 좌표(북동남서)
	static const FIntPoint Direction[DirectionCount];
};
