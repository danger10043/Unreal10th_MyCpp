// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionPlayer.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"

#include "UI/StatProgressUI.h"
#include "UI/StatProgressBar.h"
#include "Component/StatComponent.h"
#include "AnimNotify/ComboAnimNotifyState.h"
#include "Data/WeaponDataAsset.h"
#include "Weapon/WeaponActor.h"

// Sets default values
AActionPlayer::AActionPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Player);

	HornMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HornMesh"));
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftMesh"));
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightMesh"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	StatUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatUI"));

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

	HornMesh->SetupAttachment(GetMesh(), FName("LegendSocket"));
	HeadMesh->SetupAttachment(GetMesh(), FName("headSocket"));
	LeftHandMesh->SetupAttachment(GetMesh(), FName("hand_lSocket"));
	RightHandMesh->SetupAttachment(GetMesh(), FName("hand_rSocket"));
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);
	StatUIComponent->SetupAttachment(GetMesh());

	bUseControllerRotationYaw = false; // 컨트롤러 움직일 때 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동방향으로 바라보게 만들기
	SpringArmComponent->bUsePawnControlRotation = true; // 스프링암은 컨트롤러 입력에 맞게 회전

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void AActionPlayer::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* SubSystem = nullptr;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		}

		PC->PlayerCameraManager->ViewPitchMax = ViewPitchMax;
		PC->PlayerCameraManager->ViewPitchMin = ViewPitchMin;
	}

	if (SubSystem && DefaultMappingContext)
	{
		SubSystem->AddMappingContext(DefaultMappingContext, InputPriority);
	}

	AnimInstance = GetMesh()->GetAnimInstance();

	StatComponent->RunEndFunc.BindUObject(
		this,
		&AActionPlayer::OnRunEndFunction
	);

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

	if (BasicWeaponData)
	{
		IWeaponUserInterface::Execute_EquipWeapon(this, BasicWeaponData);
	}

	// GetCurrentStamina(); : 실행했을 때 C++ 에 구현된 내용만 호출한다.
	// IStaminaInterface::Execute_GetCurrentStamina(); : 실행했을 때 블루프린트 구현으로 호출한다.

}

// Called every frame
void AActionPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Log, TEXT("현재 스태미나 : %3.1f / 현재 체력 : %3.1f"), IStaminaInterface::Execute_GetCurrentStamina(StatComponent), IP0723_HealthInterface::Execute_GetCurrentHealth(StatComponent));

	FVector StatUIDirection = CameraComponent->GetComponentLocation() - StatUIComponent->GetComponentLocation();
	FRotator StatUIRotation = StatUIDirection.Rotation();
	StatUIComponent->SetWorldRotation(StatUIRotation);
}

// Called to bind functionality to input
void AActionPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* InputComponentValue = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		InputComponentValue->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionPlayer::MoveFunction);
		InputComponentValue->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AActionPlayer::LookFunction);
		InputComponentValue->BindAction(IA_Run, ETriggerEvent::Started, this, &AActionPlayer::RunStartFunction);
		InputComponentValue->BindAction(IA_Run, ETriggerEvent::Completed, this, &AActionPlayer::RunEndFunction);
		InputComponentValue->BindAction(IA_Roll, ETriggerEvent::Started, this, &AActionPlayer::RollFunction);
		InputComponentValue->BindAction(IA_Attack, ETriggerEvent::Started, this, &AActionPlayer::AttackFunction);
	}
}

void AActionPlayer::OnRunStartFunction()
{
	StatComponent->OnRunStart();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * RunMultiplier;
}

void AActionPlayer::OnRunEndFunction()
{
	StatComponent->OnRunEnd();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AActionPlayer::SetSectionJumpNotify(UComboAnimNotifyState* InSectionJumpNotify)
{
	SectionJumpNotify = InSectionJumpNotify;
	bComboReady = SectionJumpNotify.IsValid();
}

void AActionPlayer::SectionJumpForCombo()
{
	if (SectionJumpNotify.IsValid() && bComboReady)
	{
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		AnimInstance->Montage_SetNextSection(	// 섹션을 변경한다.
			AnimInstance->Montage_GetCurrentSection(CurrentMontage),	// 이 섹션에서(from)
			SectionJumpNotify->GetNextSectionName(),					// 이 섹션으로 변경(to)
			CurrentMontage	// 적용할 몽타주
		);
		GetStatComponent()->IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost));
		bComboReady = false;
	}
}

UStatComponent* AActionPlayer::GetStatComponent() const
{
	return StatComponent;
}

void AActionPlayer::MoveFunction(const FInputActionValue& InValue)
{
	FVector2D MoveAxis = InValue.Get<FVector2D>();
	FRotator CurrentControlRotation = GetControlRotation();
	FVector2D Forward = FVector2D(FMath::Cos(FMath::DegreesToRadians(CurrentControlRotation.Yaw)), FMath::Sin(FMath::DegreesToRadians(CurrentControlRotation.Yaw)));
	FVector2D Right = FVector2D(FMath::Sin(FMath::DegreesToRadians(CurrentControlRotation.Yaw)), -1 * FMath::Cos(FMath::DegreesToRadians(CurrentControlRotation.Yaw)));
	//UE_LOG(LogTemp, Log, TEXT("%.1f, %.1f"), MoveAxis.X, MoveAxis.Y);
	FVector2D MoveDirection = Forward * MoveAxis.X + Right * MoveAxis.Y * -1;
	AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, 0));
}

void AActionPlayer::LookFunction(const FInputActionValue& InValue)
{
	FVector2D LookAxis = InValue.Get<FVector2D>();
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		PC->AddYawInput(LookAxis.X);
		PC->AddPitchInput(LookAxis.Y);
	}
}

void AActionPlayer::RunStartFunction(const FInputActionValue& InValue)
{
	OnRunStartFunction();
}

void AActionPlayer::RunEndFunction(const FInputActionValue& InValue)
{
	OnRunEndFunction();
}

void AActionPlayer::RollFunction(const FInputActionValue& InValue)
{
	if (!AnimInstance->IsAnyMontagePlaying())
	{
		if (RollMontage.Get())
		{
			if (!GetLastMovementInputVector().IsNearlyZero())
			{
				SetActorRotation(GetLastMovementInputVector().Rotation());
			}

			if (StatComponent->IStaminaInterface::Execute_ConsumeStamina(StatComponent, StatComponent->GetStaminaValue(EStaminaValueType::RollCost)))
			{
				PlayAnimMontage(RollMontage.Get());
				StatComponent->StopRecover();
			}
		}
	}
}

void AActionPlayer::AttackFunction(const FInputActionValue& InValue)
{
	if (AnimInstance && GetStatComponent()->IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()) > GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost))
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			// 1번째 콤보 공격
			PlayAnimMontage(AttackMontage);
			GetStatComponent()->IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost));

			if (CurrentWeapon->AttackCount > 0)
			{
				CurrentWeapon->AttackCount--;
				if (CurrentWeapon->AttackCount == 0)
				{
					IWeaponUserInterface::Execute_EquipWeapon(this, BasicWeaponData);
				}
			}
		}
		else if (AnimInstance->GetCurrentActiveMontage() == AttackMontage)
		{
			SectionJumpForCombo();
		}
	}
}

void AActionPlayer::OnWeaponAttackState(bool bEnable)
{
	OnOnWeaponAttackStateChanged.ExecuteIfBound(bEnable);
}

void AActionPlayer::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
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

float AActionPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	StatComponent->IHealthInterface::Execute_DamageHealth(StatComponent, DamageAmount);
	return DamageAmount;
}


void AActionPlayer::SpawnWeaponActor()
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