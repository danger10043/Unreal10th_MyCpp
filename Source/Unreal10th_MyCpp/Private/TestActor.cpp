// Fill out your copyright notice in the Description page of Project Settings.

#include "TestActor.h"

// Sets default values
ATestActor::ATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// USceneComponent 을 만들어서 루트로 설정
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// UStaticMeshComponent 타입으로 디폴트 서브오브젝트를 생성하고, MainMesh 변수에 할당
	// 에디터 창에서 보이는 이름은 "MainMesh"로 설정
	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	MainMesh->SetupAttachment(Root); 
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay(); // Super = 부모 클래스
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("나는 문어입니다"));
	UE_LOG(LogTemp, Log, TEXT("BeginPlay : 이것은 문어숙회입니다"));
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay : 이것은 마지막 경고요"));
	UE_LOG(LogTemp, Error, TEXT("BeginPlay : 자살하면 죽여버리겠어!"));

	TestNormalFunction();
	TestUFunction();
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MainMesh->AddRelativeLocation(FVector(0.0f, 0.0f, MoveSpeed * DeltaTime));
}

void ATestActor::TestNormalFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("TestNormal 함수 실행"));
}

void ATestActor::TestUFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("TestUFunction 함수 실행"));
}

// 있으면 안된다.
// void ATestActor::TestImplementableFunction()
// {
// }

void ATestActor::TestNativeFunction_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("TestNativeFunction 함수 실행"));
}
