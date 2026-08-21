// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze/MazeData.h"
#include "MazeActor.generated.h"

class ACellActor;
class USceneComponent;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class UNREAL10TH_MYCPP_API AMazeActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AMazeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GenerateMaze();

	// CellActor 스폰 방식
	void SpawnCells(FMazeData* InMaze);

	// HISM 사용 방식
	void BuildMazeHISM(FMazeData* InMaze);
	void ClearHISMInstances();
	void MakeCellHISM(const FCellData* InCell, const FVector& InLocation);

	void ClearMazeActors();

	void OnPreMazeGenerate();

	void OnPostMazeGenerate();

	UFUNCTION(CallInEditor, Category = "Maze|Test")
	void TestMakeMaze();

	UFUNCTION(CallInEditor, Category = "Maze|Test")
	void TestClearMaze();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze")
	TObjectPtr<USceneComponent> SceneRoot;


	// 미로의 가로 셀 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Size",
		meta = (ClampMin = "1", UIMin = "1"))
	int32 MazeWidth = 10;

	// 미로의 세로 셀 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Size",
		meta = (ClampMin = "1", UIMin = "1"))
	int32 MazeHeight = 10;

	// CellActor 중심 사이의 간격
	// CellHalfSize가 1000이면 CellSpacing은 2000
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Size",
		meta = (ClampMin = "1.0", UIMin = "1.0"))
	float CellSpacing = 2000.0f;

	// -1이면 매번 새로운 랜덤 시드 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Spawn")
	int32 MazeSeed = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Spawn")
	EMazeAlgorithm MazeAlgorithm = EMazeAlgorithm::Wilson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Spawn")
	bool bUseHISM = true;

	// 생성된 미로의 논리 데이터
	FMazeData MazeData;


	// CellActor 관련 ===============================================================================

	// 생성할 CellActor 또는 CellActor 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|Spawn")
	TSubclassOf<ACellActor> CellActorClass;

	// 현재 월드에 생성된 셀 액터
	UPROPERTY(Transient)
	TArray<TObjectPtr<ACellActor>> SpawnedCellActors;

	// ==============================================================================================

	// HISM 사용 방식
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloorHISM = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> WallHISM = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> GateHISM = nullptr;
};
