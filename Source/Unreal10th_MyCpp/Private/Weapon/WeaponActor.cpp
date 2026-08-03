// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"
#include "Interface/StatComponentInterface.h"
#include "Data/WeaponDataAsset.h"
#include "Player/ActionPlayer.h"
#include "Component/WeaponComponent.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootMesh"));
	SetRootComponent(Mesh);
	//Mesh->SetCollisionProfileName(TEXT("NoCollision") // 프로파일을 이용해 한번에 세팅(실제 적용되는 타이밍은 조금 뒤)

	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HitArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitArea"));
	HitArea->SetGenerateOverlapEvents(true);
	HitArea->SetupAttachment(Mesh);
	HitArea->SetCapsuleHalfHeight(30.0f);
	HitArea->SetCapsuleRadius(10.0f);
	HitArea->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitArea->SetCollisionObjectType(ECC_Weapon);
	HitArea->SetCollisionResponseToChannel(ECC_Weapon, ECollisionResponse::ECR_Ignore);
	HitArea->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Ignore);
	HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Blade Trail"));
	TrailVFX->SetupAttachment(Mesh);

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);
}

void AWeaponActor::EquipToTarget(AActor* InOwner)
{
	SetOwner(InOwner);
	OwnerCharacter = Cast<ACharacter>(InOwner);
	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true
	);

	if (OwnerCharacter.IsValid())
	{
		if (AActionPlayer* Player = Cast<AActionPlayer>(OwnerCharacter))
		{
		}
		else {
			HitArea->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
			HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);
		}


		AttachToComponent(OwnerCharacter->GetMesh(), AttachRules, WeaponData->AttachSocketName);
		HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true); // 만약을 대비한 것

		if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter))
		{
			if (UWeaponComponent* WeaponComponent = WeaponUser->GetWeaponComponent())
			{
				WeaponComponent->OnWeaponAttackStateChanged.BindUFunction(this, TEXT("AttackEnable"));
			}
		}
	}
}

void AWeaponActor::DropWeapon()
{
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);

	//Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 일정 시간 동안 무기와 플레이어가 충돌 안하게 설정
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(
		PhysicsDelayTimerHandle,
		FTimerDelegate::CreateLambda(
			[this]()
			{
				Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
			}
		),
		WeaponData->PhysicsDelay,
		false
	);

	// 뒤로 던지기
	FVector BackwardDirection = -OwnerCharacter->GetActorForwardVector();
	FVector ThrowDirection = BackwardDirection * 300.0f + FVector::UpVector * 500.0f;
	Mesh->AddImpulse(ThrowDirection, NAME_None, true);
	FVector AngularImpulse = FVector(
		FMath::RandRange(-200.0f, 200.0f)
	) + GetActorForwardVector() * 1000.0f;
	Mesh->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

	// DropLifeSpan초 후에 이 액터 제거하기
	SetLifeSpan(WeaponData->DropLifeSpan);

	OwnerCharacter = nullptr;
}


void AWeaponActor::OnHitAreaBeginOverlap(
	UPrimitiveComponent* InOnComponentBeginOverlap,
	AActor* InOtherActor,
	UPrimitiveComponent* InOtherComp,
	int32					InOtherBodyIndex,
	bool					bFromSweep,
	const FHitResult& InSweepResult)
{
	if (InOtherActor && !HitActors.Contains(InOtherActor))
	{
		if (HitVFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				HitVFX,
				InOtherActor->GetActorLocation(),
				InSweepResult.ImpactNormal.Rotation()
			);
		}

		UE_LOG(LogTemp, Log, TEXT("Attack - [ %s 가 %s 를 공격 ]"), *OwnerCharacter->GetName(), *InOtherActor->GetName());
		HitActors.Add(InOtherActor);

		if (InOtherActor->GetClass()->ImplementsInterface(UStatComponentInterface::StaticClass()))
		{
			UGameplayStatics::ApplyDamage(InOtherActor, WeaponData->AttackPower, OwnerCharacter->GetController(), OwnerCharacter.Get(), nullptr);
		}
		
	}
}


void AWeaponActor::InitializeWeapon(UWeaponDataAsset* InData)
{
	if (!InData) return;

	WeaponData = InData;

	if (WeaponData->IsLoadCompleted())
	{
		Mesh->SetSkeletalMesh(WeaponData->Mesh.Get());
		Mesh->SetRelativeScale3D(WeaponData->WeaponScale);
		TrailVFX->SetAsset(WeaponData->TrailVFX.Get());
		HitVFX = WeaponData->HitVFX.Get();
		HitArea->SetCapsuleHalfHeight(WeaponData->HitAreaHalfHeight);
		HitArea->SetCapsuleRadius(WeaponData->HitAreaRadius);
		HitArea->SetRelativeLocation(WeaponData->HitAreaLocationOffset);
		HitArea->SetRelativeScale3D(WeaponData->WeaponScale);
		AttackCount = WeaponData->AttackCount;
		TrailVFX->Deactivate();
	}
}

void AWeaponActor::AttackEnable(bool bEnable)
{
	if (bEnable)
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		UE_LOG(LogTemp, Log, TEXT("Attack 활성화 됨"));
		TrailVFX->Activate();
	}
	else
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Log, TEXT("Attack 비활성화 됨"));
		HitActors.Empty();
		TrailVFX->Deactivate();
	}
}

UWeaponDataAsset* AWeaponActor::GetWeaponData() const
{
	return WeaponData;
}

int32 AWeaponActor::GetAttackCount() const
{
	return AttackCount;
}

void AWeaponActor::SetAttackCount(int32 InCount)
{
	AttackCount = InCount;
}

void AWeaponActor::IncreaseCount()
{
	AttackCount++;
}

void AWeaponActor::DecreaseCount()
{
	AttackCount--;
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

