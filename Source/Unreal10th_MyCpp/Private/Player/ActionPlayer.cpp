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
#include "UI/MainUIWidget.h"
#include "UI/InventoryUIWidget.h"
#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Component/InventoryComponent.h"
#include "AnimNotify/ComboAnimNotifyState.h"
#include "Framework/ItemFactorySubsystem.h"
#include "Item/ItemBase.h"
#include "Data/ItemDataAsset.h"

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

	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	HornMesh->SetupAttachment(GetMesh(), FName("LegendSocket"));
	HeadMesh->SetupAttachment(GetMesh(), FName("headSocket"));
	LeftHandMesh->SetupAttachment(GetMesh(), FName("hand_lSocket"));
	RightHandMesh->SetupAttachment(GetMesh(), FName("hand_rSocket"));
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

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

	if (IsLocallyControlled() && MainUIClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			MainUI = CreateWidget<UMainUIWidget>(PC, MainUIClass);

			if (IsValid(MainUI))
			{
				MainUI->SetOwnerActor(this);
				MainUI->SetInventoryComponent(InventoryComponent);
				MainUI->AddToViewport();
			}
		}
	}
	// GetCurrentStamina(); : 실행했을 때 C++ 에 구현된 내용만 호출한다.
	// IStaminaInterface::Execute_GetCurrentStamina(); : 실행했을 때 블루프린트 구현으로 호출한다.
}

// Called every frame
void AActionPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		InputComponentValue->BindAction(IA_Inventory, ETriggerEvent::Started, this, &AActionPlayer::InventoryFunction);
		InputComponentValue->BindAction(IA_UseItem, ETriggerEvent::Started, this, &AActionPlayer::UseSelectedItemFunction);
		InputComponentValue->BindAction(IA_ThrowItem, ETriggerEvent::Started, this, &AActionPlayer::ThrowSelectedItemFunction);

		for (int32 SlotIndex = 0; SlotIndex < IA_SelectInventorySlots.Num(); ++SlotIndex)
		{
			if (!IsValid(IA_SelectInventorySlots[SlotIndex])) continue;

			InputComponentValue->BindAction(IA_SelectInventorySlots[SlotIndex], ETriggerEvent::Started, this, &AActionPlayer::SelectInventorySlotFunction, SlotIndex);
		}
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
	UAnimMontage* AttackMontage = WeaponComponent ? WeaponComponent->GetAttackMontage() : nullptr;
	if (AnimInstance && AttackMontage &&
		GetStatComponent()->IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()) > GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost) &&
		WeaponComponent->GetCurrentWeapon()
		)
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		{
			// 1번째 콤보 공격
			PlayAnimMontage(AttackMontage);
			GetStatComponent()->IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), GetStatComponent()->GetStaminaValue(EStaminaValueType::AttackCost));
			WeaponComponent->ConsumeWeaponUse();
		}
		else if (AnimInstance->GetCurrentActiveMontage() == AttackMontage)
		{
			SectionJumpForCombo();
		}
	}
}

void AActionPlayer::InventoryFunction(const FInputActionValue& InValue)
{
	if (!IsValid(MainUI))
	{
		return;
	}

	MainUI->ToggleInventory();
}

void AActionPlayer::SelectInventorySlotFunction(const FInputActionValue& InValue, int32 SlotIndex)
{
	if (!IsValid(MainUI) || !MainUI->IsInventoryOpened()) return;

	MainUI->SelectInventorySlot(SlotIndex);
}

void AActionPlayer::UseSelectedItemFunction(const FInputActionValue& InValue)
{
	if (!IsValid(MainUI) || !MainUI->IsInventoryOpened() || !IsValid(InventoryComponent)) return;

	const int32 SelectedSlotIndex = MainUI->GetSelectedSlotIndex();

	if (SelectedSlotIndex == INDEX_NONE) return;

	InventoryComponent->UseItemAtIndex(SelectedSlotIndex);
}

void AActionPlayer::ThrowSelectedItemFunction(const FInputActionValue& InValue)
{
	if (!IsValid(MainUI) || !MainUI->IsInventoryOpened() || !IsValid(InventoryComponent)) return;

	const int32 SelectedSlotIndex = MainUI->GetSelectedSlotIndex();

	UItemDataAsset* ItemData = InventoryComponent->GetItemAtIndex(SelectedSlotIndex);

	if (!IsValid(ItemData))
	{
		return;
	}
	
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return;
	}

	UItemFactorySubsystem* ItemFactory = World->GetSubsystem<UItemFactorySubsystem>();
	if (!IsValid(ItemFactory))
	{
		return;
	}

	const FVector SpawnLocation = 
		GetActorLocation() 
		+ GetActorForwardVector() * 100.0f 
		+ FVector::UpVector * 100.0f;

	const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

	AItemBase* SpawnedItem = ItemFactory->SpawnItemAt(ItemData, SpawnTransform, true, GetActorForwardVector());

	if (!IsValid(SpawnedItem)) return;

	if (!InventoryComponent->RemoveItemAtIndex(SelectedSlotIndex, 1))
	{
		SpawnedItem->Destroy();
	}
}

void AActionPlayer::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
	WeaponComponent->EquipWeapon(InWeaponData);
}

float AActionPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || !IsValid(StatComponent)) return 0.0f;

	StatComponent->IHealthInterface::Execute_DamageHealth(StatComponent, DamageAmount);

	if (IsValid(MainUI))
	{
		MainUI->PlayTakeDamageAnimation();
	}
	return DamageAmount;
}
