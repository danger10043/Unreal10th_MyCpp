// Fill out your copyright notice in the Description page of Project Settings.


#include "L0722_PickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Interface/StaminaInterface.h"

// Sets default values
AL0722_PickupActor::AL0722_PickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.0f);
	SphereCollision->SetGenerateOverlapEvents(true);
	SetRootComponent(SphereCollision);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SphereCollision);
}

// Called when the game starts or when spawned
void AL0722_PickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AL0722_PickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AL0722_PickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{	
	// Actor 가 Interface를 상속받았는지 확인하는 방법 : 
	
	// Target이 null이 아니면 인터페이스를 상속받았다(=C++ 이니까 구현도 되어 있다)
	// IstaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);

	// bImplements이 true 이면 인터페이스를 구현했다.(=C++ 이니까 구현도 되어 있다. 블루프린트에서 상속을 했을 경우에는 체크가 불가능)
	// bool bImplements = OtherActor->Implements<UStaminaInterface>()

	if (OtherActor && OtherActor->Implements<UStaminaInterface>())
	{
		if (Stamina > 0)
		{
			IStaminaInterface::Execute_RecoveryStamina(OtherActor, Stamina);
		}
		else
		{
			IStaminaInterface::Execute_ConsumeStamina(OtherActor, Stamina);
		}
	}
}

