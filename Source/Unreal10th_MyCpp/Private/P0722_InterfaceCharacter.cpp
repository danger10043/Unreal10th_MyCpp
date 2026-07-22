// Fill out your copyright notice in the Description page of Project Settings.


#include "P0722_InterfaceCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"

// Sets default values
AP0722_InterfaceCharacter::AP0722_InterfaceCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HornMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HornMesh"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	HornMesh->SetupAttachment(GetMesh(), FName("LegendSocket"));
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

	bUseControllerRotationYaw = false; // 컨트롤러 움직일 때 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동방향으로 바라보게 만들기
	SpringArmComponent->bUsePawnControlRotation = true; // 스프링암은 컨트롤러 입력에 맞게 회전

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void AP0722_InterfaceCharacter::BeginPlay()
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
	CurrentStamina = MaxStamina;

	// GetCurrentStamina(); : 실행했을 때 C++ 에 구현된 내용만 호출한다.
	// IStaminaInterface::Execute_GetCurrentStamina(); : 실행했을 때 블루프린트 구현으로 호출한다.

}

// Called every frame
void AP0722_InterfaceCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Log, TEXT("현재 체력 : %f / 체력 회복까지 남은까지 남은 시간 : %f 초"), CurrentStamina, 3.0f - StaminaRecoveryWaitTime);
	if (StaminaRecoveryWaitTime < 3.0f) {
		StaminaRecoveryWaitTime += DeltaTime;
	}
	else
	{
		StaminaRecoveryWaitTime = 3.0f;
		Execute_RecoveryStamina(this, 1.0f);
	}
}

// Called to bind functionality to input
void AP0722_InterfaceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* InputComponentValue = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		InputComponentValue->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AP0722_InterfaceCharacter::MoveFunction);
		InputComponentValue->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AP0722_InterfaceCharacter::LookFunction);
		InputComponentValue->BindAction(IA_Run, ETriggerEvent::Triggered, this, &AP0722_InterfaceCharacter::RunStartFunction);
		InputComponentValue->BindAction(IA_Run, ETriggerEvent::Completed, this, &AP0722_InterfaceCharacter::RunEndFunction);
		InputComponentValue->BindAction(IA_Roll, ETriggerEvent::Started, this, &AP0722_InterfaceCharacter::RollFunction);
	}
}

void AP0722_InterfaceCharacter::MoveFunction(const FInputActionValue& InValue)
{
	FVector2D MoveAxis = InValue.Get<FVector2D>();
	FRotator CurrentControlRotation = GetControlRotation();
	FVector2D Forward = FVector2D(FMath::Cos(FMath::DegreesToRadians(CurrentControlRotation.Yaw)), FMath::Sin(FMath::DegreesToRadians(CurrentControlRotation.Yaw)));
	FVector2D Right = FVector2D(FMath::Sin(FMath::DegreesToRadians(CurrentControlRotation.Yaw)), -1 * FMath::Cos(FMath::DegreesToRadians(CurrentControlRotation.Yaw)));
	//UE_LOG(LogTemp, Log, TEXT("%.1f, %.1f"), MoveAxis.X, MoveAxis.Y);
	FVector2D MoveDirection = Forward * MoveAxis.X + Right * MoveAxis.Y * -1;
	AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, 0));
}

void AP0722_InterfaceCharacter::LookFunction(const FInputActionValue& InValue)
{
	FVector2D LookAxis = InValue.Get<FVector2D>();
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		PC->AddYawInput(LookAxis.X);
		PC->AddPitchInput(LookAxis.Y);
	}
}

void AP0722_InterfaceCharacter::RunStartFunction(const FInputActionValue& InValue)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * RunMultiplier;
	if (!Execute_ConsumeStamina(this, 1.0f))
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		StaminaRecoveryWaitTime = 0.0f;
	}
}

void AP0722_InterfaceCharacter::RunEndFunction(const FInputActionValue& InValue)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AP0722_InterfaceCharacter::RollFunction(const FInputActionValue& InValue)
{
	if (!AnimInstance->IsAnyMontagePlaying())
	{
		if (RollMontage.Get())
		{
			if (!GetLastMovementInputVector().IsNearlyZero())
			{
				SetActorRotation(GetLastMovementInputVector().Rotation());
			}

			if (Execute_ConsumeStamina(this, 20.0f))
			{
				PlayAnimMontage(RollMontage.Get());
				StaminaRecoveryWaitTime = 0.0f;
			}
		}
	}
}

float AP0722_InterfaceCharacter::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

bool AP0722_InterfaceCharacter::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;
	if (CurrentStamina >= InAmount)
	{
		CurrentStamina -= InAmount;
		bResult = true;
	}
	return bResult;
}

void AP0722_InterfaceCharacter::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0.0f, MaxStamina);
}

