// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/P0724_Player.h"
#include "UI/P0724_StatProgressUI.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Component/L0723_StatComponent.h"

// Sets default values
AP0724_Player::AP0724_Player()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HornMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HornMesh"));
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftMesh"));
	RightHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightMesh"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	StatUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatUI"));

	StatComponent = CreateDefaultSubobject<UL0723_StatComponent>(TEXT("Stat"));

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
void AP0724_Player::BeginPlay()
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
		&AP0724_Player::OnRunEndFunction
	);

	// GetCurrentStamina(); : 실행했을 때 C++ 에 구현된 내용만 호출한다.
	// IStaminaInterface::Execute_GetCurrentStamina(); : 실행했을 때 블루프린트 구현으로 호출한다.

}

// Called every frame
void AP0724_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Log, TEXT("현재 스태미나 : %3.1f / 현재 체력 : %3.1f"), IStaminaInterface::Execute_GetCurrentStamina(StatComponent), IP0723_HealthInterface::Execute_GetCurrentHealth(StatComponent));

	FVector StatUIDirection = CameraComponent->GetComponentLocation() - StatUIComponent->GetComponentLocation();
	FRotator StatUIRotation = StatUIDirection.Rotation();
	StatUIComponent->SetWorldRotation(StatUIRotation);
}

// Called to bind functionality to input
void AP0724_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* InputComponentValue = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		InputComponentValue->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AP0724_Player::MoveFunction);
		InputComponentValue->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AP0724_Player::LookFunction);
		InputComponentValue->BindAction(IA_Run, ETriggerEvent::Started, this, &AP0724_Player::RunStartFunction);
		InputComponentValue->BindAction(IA_Run, ETriggerEvent::Completed, this, &AP0724_Player::RunEndFunction);
		InputComponentValue->BindAction(IA_Roll, ETriggerEvent::Started, this, &AP0724_Player::RollFunction);
	}
}

void AP0724_Player::OnRunStartFunction()
{
	StatComponent->OnRunStart();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * RunMultiplier;
}

void AP0724_Player::OnRunEndFunction()
{
	StatComponent->OnRunEnd();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

UL0723_StatComponent* AP0724_Player::GetStatComponent() const
{
	return StatComponent;
}

void AP0724_Player::MoveFunction(const FInputActionValue& InValue)
{
	FVector2D MoveAxis = InValue.Get<FVector2D>();
	FRotator CurrentControlRotation = GetControlRotation();
	FVector2D Forward = FVector2D(FMath::Cos(FMath::DegreesToRadians(CurrentControlRotation.Yaw)), FMath::Sin(FMath::DegreesToRadians(CurrentControlRotation.Yaw)));
	FVector2D Right = FVector2D(FMath::Sin(FMath::DegreesToRadians(CurrentControlRotation.Yaw)), -1 * FMath::Cos(FMath::DegreesToRadians(CurrentControlRotation.Yaw)));
	//UE_LOG(LogTemp, Log, TEXT("%.1f, %.1f"), MoveAxis.X, MoveAxis.Y);
	FVector2D MoveDirection = Forward * MoveAxis.X + Right * MoveAxis.Y * -1;
	AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, 0));
}

void AP0724_Player::LookFunction(const FInputActionValue& InValue)
{
	FVector2D LookAxis = InValue.Get<FVector2D>();
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		PC->AddYawInput(LookAxis.X);
		PC->AddPitchInput(LookAxis.Y);
	}
}

void AP0724_Player::RunStartFunction(const FInputActionValue& InValue)
{
	OnRunStartFunction();
}

void AP0724_Player::RunEndFunction(const FInputActionValue& InValue)
{
	OnRunEndFunction();
}

void AP0724_Player::RollFunction(const FInputActionValue& InValue)
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
