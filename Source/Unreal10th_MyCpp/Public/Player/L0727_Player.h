// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/P0723_StatComponent.h"
#include "L0727_Player.generated.h"

class UInputMappingContext;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UAnimMontage;
class UAnimInstance;
class UL0723_StatComponent;
class UWidgetComponent;
class UL0727_AnimNotifyState;

UCLASS()
class UNREAL10TH_MYCPP_API AL0727_Player : public ACharacter, public IP0723_StatComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AL0727_Player();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnRunStartFunction();

	virtual void OnRunEndFunction();

	// Player Values
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewPitchMax = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ViewPitchMin = -40;

	int InputPriority = 0;

	// Notify Animation Notify (Attack Combo)
public:
	void SetSectionJumpNotify(UL0727_AnimNotifyState* InSectionJumpNotify);

private:
	// 발생한 노티파이를 저장해 놓는 변수
	TWeakObjectPtr<UL0727_AnimNotifyState> SectionJumpNotify = nullptr;

	// 현재 콤보가 가능한지
	bool bComboReady = false;

	virtual void SectionJumpForCombo();


	// Player Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> HornMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> HeadMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> LeftHandMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> RightHandMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UL0723_StatComponent> StatComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> StatUIComponent = nullptr;

	// Player Inputs
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> IA_Move = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> IA_Look = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> IA_Run = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> IA_Roll = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> IA_Attack = nullptr;

	// Player Animation
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> RollMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;



	// StatComponent Getter Function
public:
	UL0723_StatComponent* GetStatComponent() const;

	// Player Input Functions
protected:
	virtual void MoveFunction(const FInputActionValue& InValue);

	virtual void LookFunction(const FInputActionValue& InValue);

	virtual void RunStartFunction(const FInputActionValue& InValue);

	virtual void RunEndFunction(const FInputActionValue& InValue);

	virtual void RollFunction(const FInputActionValue& InValue);

	virtual void AttackFunction(const FInputActionValue& InValue);

};
