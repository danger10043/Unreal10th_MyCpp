// Fill out your copyright notice in the Description page of Project Settings.


#include "Pool/DamagePopupActor.h"
#include "Components/WidgetComponent.h"
#include "Framework/ObjectPoolSubSystem.h"
#include "UI/DamageTextWidget.h"

// Sets default values
ADamagePopupActor::ADamagePopupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Damage Popup Widget Component"));
	SetRootComponent(DamagePopupWidgetComponent);

	DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamagePopupWidgetComponent->SetDrawAtDesiredSize(true);
	DamagePopupWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
	Super::BeginPlay();

	DamagePopupWidgetComponent->InitWidget();

	DamagePopupWidget = Cast<UDamageTextWidget>(DamagePopupWidgetComponent->GetUserWidgetObject());
	if (!DamagePopupWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADamagePopupActor - DamagePopupWidgetComponent의 UserWidgetObject를 UDamageTextWidget으로 캐스팅할 수 없습니다."));
	}
	SetActorEnableCollision(false);


	PoolSubSystem = GetWorld()->GetSubsystem<UObjectPoolSubSystem>();
	if (!PoolSubSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADamagePopupActor - UObjectPoolSubSystem을 찾을 수 없습니다."));
	}
}

// Called every frame
void ADamagePopupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamagePopupActor::OnSpawnFromPool_Implementation(const FTransform& Transform)
{
	SetActorTransform(Transform);

	SetActorHiddenInGame(false);
	DamagePopupWidgetComponent->SetVisibility(true);
}

void ADamagePopupActor::OnReturnToPool_Implementation()
{
	GetWorldTimerManager().ClearTimer(ReturnToPoolTimer);

	if (DamagePopupWidget)
	{
		DamagePopupWidget->ResetDamageWidget();
	}

	DamagePopupWidgetComponent->SetVisibility(false);
	SetActorHiddenInGame(true);
}

void ADamagePopupActor::HandlePopupLifetimeEnded()
{
	if (PoolSubSystem)
	{
		PoolSubSystem->Release(this);
	}
}

void ADamagePopupActor::ShowDamage(
	float InDamage, 
	const FLinearColor& TextColor, 
	const FLinearColor& OutlineColor
)
{
	if (DamagePopupWidget)
	{
		DamagePopupWidget->PlayDamageTextAnimation(
			InDamage,
			TextColor,
			OutlineColor
		);

		GetWorldTimerManager().SetTimer(
			ReturnToPoolTimer,
			this,
			&ADamagePopupActor::HandlePopupLifetimeEnded,
			PopupLifetime,
			false
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ADamagePopupActor - DamagePopupWidget이 할당되지 않았습니다."));
	}
}

UObjectPoolSubSystem* ADamagePopupActor::GetObjectPoolSubSystem_Implementation() const
{
	return PoolSubSystem;
}

