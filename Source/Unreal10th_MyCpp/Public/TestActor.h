// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// generated.h는 항상 마지막에 include 해야 한다.
#include "TestActor.generated.h"

// UCLASE()
// - C++ 클래스를 언리얼 엔진의 객체 시스템에 등록(= UObject)
// - 가비지 콜렉터 지원, 블루프린트 연동, 에디터에서 노출 등이 가능해짐
// - 다중 상속을 막는다.

// CDO (Class Defult Object)
// - 기본 클래스 인스턴스. 새 클래스가 인스턴싱될 때 CDO를 복사해서 사용.

UCLASS()
class UNREAL10TH_MYCPP_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;  // Tick은 protected로 변경

	void TestNormalFunction(); // 이렇게 선언된 함수는 C++에서만 접근 가능, 블루프린트에서는 접근 불가

	UFUNCTION(BlueprintCallable) // 이렇게 선언된 함수는 C++과 블루프린트에서 모두 접근 가능
	void TestUFunction(); // 이렇게 선언된 함수는 C++과 블루프린트에서 모두 접근 가능

	//UFUNCTION(BlueprintImplementableEvent); // C++ 에서 선언만하고 구현은 블루프린트에서 하겠다.
	//void TestImplementableFunction();

	UFUNCTION(BlueprintNativeEvent) // C++ 에서 선언하고 구현도 하겠다. 블루프린트에서 오버라이드도 가능
	void TestNativeFunction(); // 구현은 _Implementation() 를 붙여서 해야 한다. (호출은 원래 이름으로 한다.)

protected:
	int32 Data1 = 10; // 이렇게 선언된 변수는 C++에서만 접근 가능, 블루프린트에서는 접근 불가

	// UPROPERTY : 클래스나 구조체의 멤버 변수를 언리얼 엔진에 등록하는 것(리플렉션 지원, 가비지 콜렉팅 지원)
	UPROPERTY(VisibleAnywhere) // 클래스 Default 창과, 맵에 배치된 액터의 Details 창에서 모두 확인 가능
	int32 DataVisibaleAnywhere = 20;

	UPROPERTY(VisibleDefaultsOnly) // 클래스 Default 창에서만 확인 가능
	int32 DataVisibleDefaultsOnly = 21;

	UPROPERTY(VisibleInstanceOnly) // 맵에 배치된 액터의 Details 창에서만 확인 가능
	int32 DataVisibleInstanceOnly = 22;

	UPROPERTY(EditAnywhere); // 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 수정 가능
	int32 DataEditAnywhere = 30;

	UPROPERTY(EditDefaultsOnly) // 클래스 디폴트 창에서만 수정 가능
	int32 DataEditDefaultsOnly = 31;

	UPROPERTY(EditInstanceOnly) // 맵에 배치된 액터의 디테일 창에서만 수정 가능
	int32 DataEditInstanceOnly = 32;

	UPROPERTY(BlueprintReadOnly) // 블루프린트에서 읽기만 가능
	int32 DataBlueprintReadOnly = 40;

	UPROPERTY(BlueprintReadOnly, EditAnywhere) // UPROPERTY는 여러 개를 동시에 사용할 수 있음
	int32 DataBlueprintReadOnlyAndEditAnywhere = -10;

	UPROPERTY(BlueprintReadWrite) // 블루프린트에서 읽기&쓰기 가능
	int32 DataBlueprintReadWrite = 50;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) // 블루프린트에서 읽기&쓰기가 가능하고, 디테일	창에서도 수정 가능
	int32 DataBlueprintReadWriteAndEditAnywhere = -20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MyCategory") // 카테고리 지정 가능
	int32 CategoryData = 60;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MyCategory|SubCategory") // 서브 카테고리도 지정 가능
	int32 SubCategoryData = 70;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MainMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 10.0f;
};
