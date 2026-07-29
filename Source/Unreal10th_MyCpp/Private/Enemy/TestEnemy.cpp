// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TestEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Component/StatComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/StatProgressUI.h"
#include "UI/StatProgressBar.h"
#include "Camera/CameraComponent.h"
#include "Player/ActionPlayer.h"

// Sets default values
ATestEnemy::ATestEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightMesh"));
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));
	StatUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatUI"));

	RightHandMesh->SetupAttachment(GetMesh(), FName("hand_rSocket"));
	StatUIComponent->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void ATestEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();

	TargetPlayer = Cast<AActionPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (UStatProgressUI* UI = Cast<UStatProgressUI>(StatUIComponent->GetUserWidgetObject()))
	{
		UI->SetOwnerActor(this);

		UI->HealthProgressBar->SetProgressBarColor(FLinearColor(1.0f, 0.1f, 0.1f, 1.0f));
		UI->StaminaProgressBar->SetProgressBarColor(FLinearColor(0.1f, 1.0f, 0.1f, 1.0f));

		StatComponent->OnHealthChange.AddDynamic(UI, &UStatProgressUI::SetHealth);
		StatComponent->OnStaminaChange.AddDynamic(UI, &UStatProgressUI::SetStamina);

		float MaxHealth = StatComponent->IHealthInterface::Execute_GetMaxHealth(StatComponent);
		float MaxStamina = StatComponent->IStaminaInterface::Execute_GetMaxStamina(StatComponent);
		UI->SetHealth(MaxHealth, MaxHealth);
		UI->SetStamina(MaxStamina, MaxStamina);
	}

	UWorld* World = GetWorld();
	FTimerManager& TimerManager = World->GetTimerManager();

	TimerManager.SetTimer(
		AttackCooldownTimer,
		FTimerDelegate::CreateLambda(
			[this]()
			{
				AttackFunction();
			}),
		3.0f,
		true
	);

	AnimInstance = GetMesh()->GetAnimInstance();
}

// Called every frame
void ATestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(InitialLocation);
	SetActorRotation(InitialRotation);
	UCameraComponent* CameraComponent = TargetPlayer->CameraComponent;
	FVector StatUIDirection = CameraComponent->GetComponentLocation() - StatUIComponent->GetComponentLocation();
	FRotator StatUIRotation = StatUIDirection.Rotation();
	StatUIComponent->SetWorldRotation(StatUIRotation);
}

// Called to bind functionality to input
void ATestEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UStatComponent* ATestEnemy::GetStatComponent() const
{
	return StatComponent;
}

float ATestEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	StatComponent->IHealthInterface::Execute_DamageHealth(StatComponent, DamageAmount);
	return DamageAmount;
}

void ATestEnemy::OnWeaponAttackState(bool bEnable)
{
	OnOnWeaponAttackStateChanged.ExecuteIfBound(bEnable);
}

void ATestEnemy::AttackFunction()
{
	UE_LOG(LogTemp, Log, TEXT("적 : 핫하 받아라!"));
	if (AnimInstance && GetStatComponent()->IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()) > GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost))
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			// 1번째 콤보 공격
			PlayAnimMontage(AttackMontage);
			GetStatComponent()->IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost));
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("공격 실패"));
		if (!AnimInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("AnimInstance 존재 안함"));
		}
		UE_LOG(LogTemp, Log, TEXT("Current Stamina : %f"), GetStatComponent()->IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()));
		UE_LOG(LogTemp, Log, TEXT("Need Stamina : %f"), GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost));
	}
}
