// ItemStamina.cpp 신규 파일

#include "Item/ItemStamina.h"

#include "Component/StatComponent.h"
#include "Data/ItemDataAsset.h"
#include "Interface/StatComponentInterface.h"
#include "Interface/StaminaInterface.h"

AItemStamina::AItemStamina()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemStamina::InitializeFromItemData(
	UItemDataAsset* InItemData)
{
	Super::InitializeFromItemData(InItemData);

	if (!IsValid(InItemData))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemStamina::InitializeFromItemData - ItemData 가 유효하지 않습니다."));
		return;
	}

	Stamina = InItemData->StatAmount;
}

float AItemStamina::GetStamina() const
{
	return Stamina;
}

float AItemStamina::GetStaminaTickInterval() const
{
	return StaminaTickInterval;
}

void AItemStamina::SetStamina(float InStamina)
{
	Stamina = InStamina;
}

void AItemStamina::SetStaminaTickInterval(float InStaminaTickInterval)
{
	StaminaTickInterval = InStaminaTickInterval;
}

void AItemStamina::PickUpItem(AActor* InActor)
{
    if (IStatComponentInterface* Player = Cast<IStatComponentInterface>(InActor))
    {
        UWorld* World = GetWorld();
        FTimerManager& TimerManager = World->GetTimerManager();
        UStatComponent* StatComponent = Player->GetStatComponent();

        TimerManager.ClearTimer(ItemMeshBrightnessDecreaseTimerHandle);

        TimerManager.SetTimer(
            StaminaTickTimer,
            FTimerDelegate::CreateLambda(
                [this, StatComponent, InActor]()
                {
                    if (Stamina > 0.0f) {
                        StatComponent->IStaminaInterface::Execute_RecoveryStamina(StatComponent, Stamina);
                        SpawnPickupNiagaraEffect(InActor);
                    }
                    else {
                        StatComponent->IStaminaInterface::Execute_ConsumeStamina(StatComponent, -Stamina);
                    }
                }
            ),
            StaminaTickInterval,
            true
        );

        TimerManager.SetTimer(
            ItemMeshRotationTimerHandle,
            FTimerDelegate::CreateLambda(
                [&]()
                {
                    ItemStaticMesh->AddRelativeRotation(FRotator(0.0f, 4.5f, 0.0f));
                }
            ),
            0.01f,
            true
        );

        TimerManager.SetTimer(
            ItemMeshBrightnessIncreaseTimerHandle,
            FTimerDelegate::CreateLambda(
                [&]()
                {
                    CurrentMeshBright += MeshBrightChangeAmount;
                    if (MeshMID)
                    {
                        MeshMID->SetScalarParameterValue(MIDBrightnessName, CurrentMeshBright);
                    }

                    if (CurrentMeshBright >= MaxMeshBright)
                    {
                        TimerManager.ClearTimer(ItemMeshBrightnessIncreaseTimerHandle);
                    }
                }
            ),
            0.03f,
            true
        );
    }
}

void AItemStamina::BeginPlay()
{
    Super::BeginPlay();
    MeshMID = ItemStaticMesh->CreateDynamicMaterialInstance(0);

    if (!MeshMID)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s의 Dynamic Material Instance 생성에 실패했습니다."), *this->GetName());
    }
}

void AItemStamina::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItemStamina::pickDownItem(AActor* InActor)
{
    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();
    TimerManager.ClearTimer(StaminaTickTimer);
    TimerManager.ClearTimer(ItemMeshBrightnessIncreaseTimerHandle);
    TimerManager.ClearTimer(ItemMeshRotationTimerHandle);

    TimerManager.SetTimer(
        ItemMeshBrightnessDecreaseTimerHandle,
        FTimerDelegate::CreateLambda(
            [&]()
            {
                CurrentMeshBright -= MeshBrightChangeAmount;
                if (MeshMID)
                {
                    MeshMID->SetScalarParameterValue(MIDBrightnessName, CurrentMeshBright);
                }

                if (CurrentMeshBright <= MinMeshBright)
                {
                    TimerManager.ClearTimer(ItemMeshBrightnessDecreaseTimerHandle);
                }
            }
        ),
        0.03f,
        true
    );
}
