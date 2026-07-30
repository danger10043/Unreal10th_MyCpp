// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/EnemyWeaponActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"
#include "Interface/StatComponentInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyWeaponActor::AEnemyWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	SetRootComponent(Mesh);
	//Mesh->SetCollisionProfileName(TEXT("NoCollision") // 프로파일을 이용해 한번에 세팅(실제 적용되는 타이밍은 조금 뒤)

	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HitArea = CreateDefaultSubobject<UBoxComponent>(TEXT("HitArea"));
	HitArea->SetGenerateOverlapEvents(true);
	HitArea->SetupAttachment(Mesh);
	HitArea->SetBoxExtent(FVector(100.0f, 35.0f, 160.0f));
	HitArea->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f));
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitArea->SetCollisionObjectType(ECC_Weapon);
	HitArea->SetCollisionResponseToChannel(ECC_Weapon, ECollisionResponse::ECR_Ignore);
	HitArea->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void AEnemyWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	HitArea->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWeaponActor::OnHitAreaBeginOverlap);

}

void AEnemyWeaponActor::OnEquipped(AActor* InOwner)
{
	SetOwner(InOwner);
	OwnerCharacter = Cast<ACharacter>(InOwner);
	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);

	if (OwnerCharacter.IsValid())
	{
		AttachToComponent(OwnerCharacter->GetMesh(), AttachRules, AttachSocketName);
		HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true); // 만약을 대비한 것

		IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter);
		WeaponUser->GetWeaponAttackStateChangedDelegate().BindUFunction(this, TEXT("AttackEnable"));
		UE_LOG(LogTemp, Log, TEXT("무기가 적에게 Equip 되었음"));
	}
}

void AEnemyWeaponActor::OnHitAreaBeginOverlap(
	UPrimitiveComponent* InOnComponentBeginOverlap,
	AActor* InOtherActor,
	UPrimitiveComponent* InOtherComp,
	int32					InOtherBodyIndex,
	bool					bFromSweep,
	const FHitResult& InSweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("오버랩 된 대상 : %s"), *InOtherActor->GetName());

	if (IStatComponentInterface* VictimActor = Cast<IStatComponentInterface>(InOtherActor))
	{
		UGameplayStatics::ApplyDamage(InOtherActor, WeaponDamage, OwnerCharacter->GetController(), OwnerCharacter.Get(), nullptr);
	}
}

void AEnemyWeaponActor::AttackEnable(bool bEnable)
{
	if (bEnable)
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		UE_LOG(LogTemp, Log, TEXT("Attack 활성화 됨"));
	}
	else
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Log, TEXT("Attack 비활성화 됨"));
	}
}

// Called every frame
void AEnemyWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

