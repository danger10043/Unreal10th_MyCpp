// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"
#include "Animation/AnimInstance.h"

#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "UI/StatProgressUI.h"
#include "UI/StatProgressBar.h"
#include "Camera/CameraComponent.h"
#include "Player/ActionPlayer.h"
#include "Item/ItemWeapon.h"
#include "Data/WeaponDataAsset.h"
#include "Data/ItemDataAsset.h"
#include "Weapon/WeaponActor.h"
#include "Framework/ItemFactorySubsystem.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat Component"));
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightMesh"));
	StatUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatUI"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));

	RightHandMesh->SetupAttachment(GetMesh(), FName("hand_rSocket"));
	StatUIComponent->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	StatComponent->OnDie.AddDynamic(this, &AEnemyCharacter::OnDie);

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
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SetActorLocation(InitialLocation);
	//SetActorRotation(InitialRotation);

	if (TargetPlayer.IsValid())
	{
		UCameraComponent* CameraComponent = TargetPlayer->CameraComponent;
		FVector StatUIDirection = CameraComponent->GetComponentLocation() - StatUIComponent->GetComponentLocation();
		FRotator StatUIRotation = StatUIDirection.Rotation();
		StatUIComponent->SetWorldRotation(StatUIRotation);
	}
}

void AEnemyCharacter::OnDie()
{
	UE_LOG(LogTemp, Log, TEXT("%s가 사망했습니다."), *GetName());
	DropItems();
}

UStatComponent* AEnemyCharacter::GetStatComponent() const
{
	return StatComponent;
}

void AEnemyCharacter::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
	WeaponComponent->EquipWeapon(InWeaponData);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	StatComponent->IHealthInterface::Execute_DamageHealth(StatComponent, DamageAmount);
	return DamageAmount;
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnDieAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DieAnimation) {
		SpawnDropItemAndDestroy();
	}
}

void AEnemyCharacter::SpawnDropItemAndDestroy()
{
	const FItemDropEntry* SelectedEntry = nullptr;


	for (const FItemDropEntry& Entry : ItemDropTable)
	{
		if (!IsValid(Entry.ItemDataAsset) || Entry.Probability <= 0.0f)
		{
			continue;
		}

		const float Roll = FMath::FRand();

		if (Roll <= Entry.Probability)
		{
			SelectedEntry = &Entry;

			if (SelectedEntry && GetWorld() && GetMesh())
			{
				const FVector DropLocation = GetMesh()->GetComponentLocation() + FVector(0.0f, 0.0f, 130.f);

				const FTransform SpawnTransform(
					GetActorRotation(),
					DropLocation
				);

				UE_LOG(LogTemp, Log, TEXT("EnemyCharacter::SpawnDropitemAndDestroy - Enemy DropLocation : %s"), *DropLocation.ToString());

				if (UItemFactorySubsystem* ItemFactory = GetWorld()->GetSubsystem<UItemFactorySubsystem>())
				{
					ItemFactory->SpawnItemAt(
						SelectedEntry->ItemDataAsset,
						SpawnTransform
					);
				}
			}
		}
	}


	if (WeaponComponent)
	{
		if (AWeaponActor* EquippedWeapon = WeaponComponent->GetCurrentWeapon())
		{
			EquippedWeapon->Destroy();
		}
	}

	Destroy();
}

void AEnemyCharacter::AttackFunction()
{
	UAnimMontage* AttackMontage = WeaponComponent ? WeaponComponent->GetAttackMontage() : nullptr;
	if (AnimInstance && AttackMontage &&
		WeaponComponent->GetCurrentWeapon() &&
		GetStatComponent()->IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()) > GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost))
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			// 1번째 콤보 공격
			PlayAnimMontage(AttackMontage);
			GetStatComponent()->IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost));
			WeaponComponent->ConsumeWeaponUse();
		}
	}
	else {
		//UE_LOG(LogTemp, Log, TEXT("공격 실패"));
		if (!AnimInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("AnimInstance 존재 안함"));
		}
	}
}

void AEnemyCharacter::DropItems()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	GetWorldTimerManager().ClearTimer(AttackCooldownTimer);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AnimInstance && DieAnimation && PlayAnimMontage(DieAnimation) > 0.0f)
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AEnemyCharacter::OnDieAnimationEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, DieAnimation);
		return;
	}

	SpawnDropItemAndDestroy();
}

