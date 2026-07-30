// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/TestEnemy.h"
#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"
#include "Kismet/GameplayStatics.h"
#include "Component/StatComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/StatProgressUI.h"
#include "UI/StatProgressBar.h"
#include "Camera/CameraComponent.h"
#include "Player/ActionPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Data/WeaponDataAsset.h"
#include "Weapon/WeaponActor.h"

// Sets default values
ATestEnemy::ATestEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);

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

	if (WeaponData)
	{
		UE_LOG(LogTemp, Log, TEXT("%s 가 WeaponData로 무기 장착을 시도함"), *this->GetName());
		IWeaponUserInterface::Execute_EquipWeapon(this, WeaponData);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s 에게 WeaponData가 존재하지 않음"), *this->GetName());
	}
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

void ATestEnemy::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
	UE_LOG(LogTemp, Log, TEXT("%s 가 무기 착용을 시도함"), *this->GetName());

	// 이전 무기 해제하기
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon.Get()->DropWeapon();
		CurrentWeapon = nullptr;
	}

	// 새 무기 장비하기
	CurrentWeaponData = InWeaponData;
	if (!InWeaponData->IsLoadCompleted())
	{
		UWeaponDataAsset* RequestedData = InWeaponData;
		InWeaponData->RequestDataLoad(
			FStreamableDelegate::CreateWeakLambda(
				this,
				[this, RequestedData]()
				{
					// 로딩이 완료되면 실행되는 람다 함수					
					if (CurrentWeaponData == RequestedData)
					{
						// 중복으로 로딩 요청했을 때를 대비
						SpawnWeaponActor();
					}
				})
		);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("WeaponData 이미 로드 완료됨"));
		SpawnWeaponActor();
	}
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

void ATestEnemy::SpawnWeaponActor()
{
	if (!CurrentWeaponData)
	{
		return;	// 로딩 요청이 끝나기 전에 해제되었을 때를 대비
	}

	CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(
		AWeaponActor::StaticClass(), FTransform::Identity, this, this);	// 스폰 시작
	if (CurrentWeapon.IsValid())
	{
		CurrentWeapon->InitializeWeapon(CurrentWeaponData);
		UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);	// 스폰 완료(=BeginPlay까지 실행)
		CurrentWeapon->EquipToTarget(this);
	}
}