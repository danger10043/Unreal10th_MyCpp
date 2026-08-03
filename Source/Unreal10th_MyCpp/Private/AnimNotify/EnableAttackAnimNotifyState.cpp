// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/EnableAttackAnimNotifyState.h"
#include "GameFramework/Character.h"
#include "Player/ActionPlayer.h"
#include "Interface/WeaponUserInterface.h"
#include "Component/WeaponComponent.h"

void UEnableAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(MeshComp->GetOwner()))
	{
		if (UWeaponComponent* WeaponComponent = WeaponUser->GetWeaponComponent())
		{
			WeaponComponent->OnWeaponAttackStateChanged.ExecuteIfBound(true);
		}
	}
}

void UEnableAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(MeshComp->GetOwner()))
	{
		if (UWeaponComponent* WeaponComponent = WeaponUser->GetWeaponComponent())
		{
			WeaponComponent->OnWeaponAttackStateChanged.ExecuteIfBound(false);
		}
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
