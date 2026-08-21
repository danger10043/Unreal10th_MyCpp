// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"
#include "Unreal10th_MyCpp/Unreal10th_MyCpp.h"

#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Component/InventoryComponent.h"
#include "Data/ItemDataAsset.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DropPhysicsCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Drop Physics Collision"));
	DropPhysicsCollision->InitSphereRadius(100.0f);
	DropPhysicsCollision->SetCollisionObjectType(ECC_PhysicsBody);
	DropPhysicsCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DropPhysicsCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DropPhysicsCollision->SetNotifyRigidBodyCollision(true);
	DropPhysicsCollision->SetGenerateOverlapEvents(false);
	SetRootComponent(DropPhysicsCollision);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->InitSphereRadius(100.0f);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereCollision->SetupAttachment(DropPhysicsCollision);

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Static Mesh"));
	ItemStaticMesh->SetupAttachment(DropPhysicsCollision);
	ItemStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemStaticMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemStaticMesh->SetGenerateOverlapEvents(false);
	ItemStaticMesh->SetSimulatePhysics(false);
	ItemStaticMesh->SetCanEverAffectNavigation(false);

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Skeletal Mesh"));
	ItemSkeletalMesh->SetupAttachment(DropPhysicsCollision);
	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemSkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemSkeletalMesh->SetGenerateOverlapEvents(false);
	ItemSkeletalMesh->SetSimulatePhysics(false);
	ItemSkeletalMesh->SetAllBodiesSimulatePhysics(false);
	ItemSkeletalMesh->SetCanEverAffectNavigation(false);

	SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Overlap);
	SphereCollision->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Ignore);

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Component"));
	NiagaraComponent->SetupAttachment(DropPhysicsCollision);
	NiagaraComponent->SetAutoActivate(false);
}

void AItemBase::InitializeFromItemData(UItemDataAsset* InItemData)
{
	if (!IsValid(InItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBase::InitializeFromItemData - ItemData가 유효하지 않습니다."));
		return;
	}
	ItemData = InItemData;

	if (IsValid(NiagaraComponent))
	{
		UNiagaraSystem* LoadedIdleNiagara = ItemData->IdleNiagaraSystem.LoadSynchronous();

		NiagaraComponent->SetAsset(LoadedIdleNiagara);

		if (IsValid(LoadedIdleNiagara))
		{
			NiagaraComponent->Activate(true);
		}
		else
		{
			NiagaraComponent->Deactivate();
		}
	}

	PickupNiagaraSystem = ItemData->PickupNiagaraSystem.LoadSynchronous();

	ItemStaticMesh->SetStaticMesh(nullptr);
	ItemStaticMesh->SetVisibility(false);

	ItemSkeletalMesh->SetSkeletalMesh(nullptr);
	ItemSkeletalMesh->SetVisibility(false);

	if (!ItemData->SkeletalMesh.IsNull())
	{
		USkeletalMesh* LoadedSkeletalMesh = ItemData->SkeletalMesh.LoadSynchronous();

		if (IsValid(LoadedSkeletalMesh))
		{
			ItemSkeletalMesh->SetSkeletalMesh(LoadedSkeletalMesh);
			ItemSkeletalMesh->SetRelativeScale3D(ItemData->MeshScale);
			ItemSkeletalMesh->SetRelativeRotation(ItemData->MeshRotation);
			ItemSkeletalMesh->SetVisibility(true);
			return;
		}
	}

	if (!ItemData->StaticMesh.IsNull())
	{
		UStaticMesh* LoadedStaticMesh = ItemData->StaticMesh.LoadSynchronous();

		if (IsValid(LoadedStaticMesh))
		{
			ItemStaticMesh->SetStaticMesh(LoadedStaticMesh);
			ItemStaticMesh->SetRelativeScale3D(ItemData->MeshScale);
			ItemStaticMesh->SetRelativeRotation(ItemData->MeshRotation);
			ItemStaticMesh->SetVisibility(true);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ItemBase::InitializeFromItemData - 표시할 Mesh가 없습니다. Item=%s"),
		*ItemData->GetName());
}

void AItemBase::PrePareForDropSpawn()
{
	if (IsValid(SphereCollision))
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AItemBase::LaunchAsDroppedItem(
	const bool bDoLaunchToForwardOfPlayer,
	const FVector LaunchDirection
)
{
	if (!IsValid(DropPhysicsCollision) || !IsValid(SphereCollision))
	{
		return;
	}
	bHasSettled = false;
	bPickupDelayElapsed = false;

	GetWorldTimerManager().ClearTimer(PickupDelayTimerHandle);

	GetWorldTimerManager().SetTimer(
		PickupDelayTimerHandle,
		FTimerDelegate::CreateWeakLambda(
			this,
			[this]()
			{
				bPickupDelayElapsed = true;
				EnablePickupCollisionIfReady();
			}
		),
		DropPickupDelay,
		false
	);

	if (IsValid(ItemStaticMesh))
	{
		ItemStaticMesh->SetSimulatePhysics(false);

		ItemStaticMesh->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);

		ItemStaticMesh->SetCollisionResponseToAllChannels(
			ECollisionResponse::ECR_Ignore
		);

		ItemStaticMesh->SetGenerateOverlapEvents(false);
	}

	if (IsValid(ItemSkeletalMesh))
	{
		ItemSkeletalMesh->SetSimulatePhysics(false);
		ItemSkeletalMesh->SetAllBodiesSimulatePhysics(false);

		ItemSkeletalMesh->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);

		ItemSkeletalMesh->SetCollisionResponseToAllChannels(
			ECollisionResponse::ECR_Ignore
		);

		ItemSkeletalMesh->SetGenerateOverlapEvents(false);
	}

	// 날아가는 도중 플레이어가 아이템을 획득하지 못하게 합니다.
	SphereCollision->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	DropPhysicsCollision->SetCollisionObjectType(
		ECC_PhysicsBody
	);

	DropPhysicsCollision->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics
	);

	DropPhysicsCollision->SetCollisionResponseToAllChannels(
		ECollisionResponse::ECR_Ignore
	);

	// 일반적인 바닥과 벽
	DropPhysicsCollision->SetCollisionResponseToChannel(
		ECC_WorldStatic,
		ECollisionResponse::ECR_Block
	);

	// 움직이는 발판이나 Blueprint 장애물
	DropPhysicsCollision->SetCollisionResponseToChannel(
		ECC_WorldDynamic,
		ECollisionResponse::ECR_Block
	);

	// 다른 드롭 아이템과 서로 충돌하지 않도록 합니다.
	DropPhysicsCollision->SetCollisionResponseToChannel(
		ECC_PhysicsBody,
		ECollisionResponse::ECR_Ignore
	);

	// 플레이어와 Enemy는 드롭 이동 중 무시
	DropPhysicsCollision->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECollisionResponse::ECR_Ignore
	);

	DropPhysicsCollision->SetCollisionResponseToChannel(
		ECC_Player,
		ECollisionResponse::ECR_Ignore
	);

	DropPhysicsCollision->SetCollisionResponseToChannel(
		ECC_Enemy,
		ECollisionResponse::ECR_Ignore
	);

	DropPhysicsCollision->SetSimulatePhysics(true);
	DropPhysicsCollision->SetEnableGravity(true);

	FVector2D RandomDirection2D;
	if (bDoLaunchToForwardOfPlayer)
	{
		RandomDirection2D = FVector2D(LaunchDirection.X, LaunchDirection.Y).GetRotated(FMath::FRandRange(-10.0f, 10.0f));
	}
	else
	{
		RandomDirection2D = FMath::RandPointInCircle(1.0f).GetSafeNormal();
	}

	const float HorizontalSpeed =
		FMath::FRandRange(
			DropHorizontalSpeedRange.X,
			DropHorizontalSpeedRange.Y
		);

	const float VerticalSpeed =
		FMath::FRandRange(
			DropVerticalSpeedRange.X,
			DropVerticalSpeedRange.Y
		);

	const FVector LaunchVelocity(
		RandomDirection2D.X * HorizontalSpeed,
		RandomDirection2D.Y * HorizontalSpeed,
		VerticalSpeed
	);

	// bVelChange=true이므로 질량에 상관없이 속도 변화로 적용됩니다.
	DropPhysicsCollision->AddImpulse(
		LaunchVelocity,
		NAME_None,
		true
	);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	InitialMeshLocation = ItemStaticMesh->GetComponentLocation();

	if (IsValid(DropPhysicsCollision))
	{
		DropPhysicsCollision->OnComponentHit.AddDynamic(
			this,
			&AItemBase::OnDropPhysicsHit
		);
	}
}

void AItemBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	PickUpItem(OtherActor);
}

void AItemBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	pickDownItem(OtherActor);

	Super::NotifyActorEndOverlap(OtherActor);
}

void AItemBase::PickUpItem(AActor* InActor)
{
	if (!IsValid(InActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBase::PickUpItem - 접촉 액터가 유효하지 않습니다."));
		return;
	}
	if (!IsValid(ItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBase::PickUpItem - ItemData가 유효하지 않습니다."));
		return;
	}

	UInventoryComponent* InventoryComponent = InActor->FindComponentByClass<UInventoryComponent>();

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBase::PickUpItem - 접촉 액터가 인벤토리 컴포넌트를 가지고 있지 않습니다."));
		return;
	}

	if (!InventoryComponent->AddItem(ItemData, 1))
	{
		return;
	}
	
	SetActorEnableCollision(false);
	SpawnPickupNiagaraEffect(InActor);

	UE_LOG(LogTemp, Log, TEXT("%s 가 %s 아이템을 획득했습니다."), *InActor->GetName(), *ItemData->DisplayName.ToString());

	Destroy();
}

void AItemBase::pickDownItem(AActor* InActor)
{
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElapsedTime += DeltaTime;

	if (ItemStaticMesh)
	{
		ItemStaticMesh->AddRelativeRotation(FRotator(0.0f, 30.0f * DeltaTime, 0.0f));
		ItemStaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 5 * FMath::Cos(ElapsedTime * 0.7)));
	}

	if (ItemSkeletalMesh)
	{
		ItemSkeletalMesh->AddRelativeRotation(FRotator(0.0f, 30.0f * DeltaTime, 0.0f));
		ItemSkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 5 * FMath::Cos(ElapsedTime * 0.7)));
	}
}

void AItemBase::SpawnPickupNiagaraEffect(AActor* InActor)
{
	if (!IsValid(PickupNiagaraSystem))
	{
		return;
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		PickupNiagaraSystem,
		InActor->GetActorLocation(),
		InActor->GetActorRotation()
	);
}

void AItemBase::EnablePickupCollisionIfReady()
{
	if (!bHasSettled || !bPickupDelayElapsed || !IsValid(SphereCollision)) return;

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AItemBase::OnDropPhysicsHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasSettled || !IsValid(OtherComponent))
	{
		return;
	}

	const ECollisionChannel OtherObjectType = OtherComponent->GetCollisionObjectType();

	if (OtherObjectType != ECC_WorldStatic &&
		OtherObjectType != ECC_WorldDynamic)
	{
		return;
	}

	FinishDropMovement();
}

void AItemBase::FinishDropMovement()
{
	if (bHasSettled)
	{
		return;
	}

	bHasSettled = true;

	if (IsValid(DropPhysicsCollision))
	{
		DropPhysicsCollision->SetPhysicsLinearVelocity(
			FVector::ZeroVector
		);

		DropPhysicsCollision->SetPhysicsAngularVelocityInDegrees(
			FVector::ZeroVector
		);

		DropPhysicsCollision->SetSimulatePhysics(false);

		DropPhysicsCollision->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);
	}

	EnablePickupCollisionIfReady();
}

void AItemBase::MoveVFXtoStaticMesh()
{
	NiagaraComponent->SetWorldLocation(ItemStaticMesh->GetComponentLocation());
}

void AItemBase::MoveVFXtoSkeletalMesh()
{
	NiagaraComponent->SetWorldLocation(ItemSkeletalMesh->GetComponentLocation());
}

