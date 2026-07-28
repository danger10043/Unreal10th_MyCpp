// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Lecture0721_ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ALecture0721_ActionCharacter::ALecture0721_ActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent);

	bUseControllerRotationYaw = false; // 컨트롤러 움직일 때 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동방향으로 바라보게 만들기
	CameraSpringArmComponent->bUsePawnControlRotation = true; // 스프링암은 컨트롤러 입력에 맞게 회전

	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

// Called when the game starts or when spawned
void ALecture0721_ActionCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALecture0721_ActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALecture0721_ActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &ALecture0721_ActionCharacter::OnTestAction);
		UE_LOG(LogTemp, Log, TEXT("바인드 완료"));

		EnhancedInputComponent->BindAction(IA_Boost, ETriggerEvent::Started, this, &ALecture0721_ActionCharacter::OnBoostOnAction);
		EnhancedInputComponent->BindAction(IA_Boost, ETriggerEvent::Completed, this, &ALecture0721_ActionCharacter::OnBoostOffAction);
	}
}

void ALecture0721_ActionCharacter::OnTestAction(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("TestAction 실행"));

}

void ALecture0721_ActionCharacter::OnBoostOnAction(const FInputActionValue& Value)
{
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed * 3;
}

void ALecture0721_ActionCharacter::OnBoostOffAction(const FInputActionValue& Value)
{
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

