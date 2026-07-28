// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/L0727_AnimNotifyState.h"
#include "Player/L0727_Player.h"

void UL0727_AnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	OwnerCharacter = Cast<AL0727_Player>(MeshComp->GetOwner());
	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->SetSectionJumpNotify(this);
	}
}

void UL0727_AnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->SetSectionJumpNotify(nullptr);
		OwnerCharacter = nullptr;
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
