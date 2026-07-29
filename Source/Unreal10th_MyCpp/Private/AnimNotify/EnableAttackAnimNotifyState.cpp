// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/EnableAttackAnimNotifyState.h"
#include "GameFramework/Character.h"
#include "Player/ActionPlayer.h"
#include "Interface/WeaponUserInterface.h"

void UEnableAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	OwnerCharacter = Cast<IWeaponUserInterface>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->OnWeaponAttackState(true);
	}
}

void UEnableAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->OnWeaponAttackState(false);
		OwnerCharacter = nullptr;
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
