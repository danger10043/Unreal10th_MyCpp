// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ComboAnimNotifyState.h"
#include "Player/ActionPlayer.h"

void UComboAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActionPlayer* OwnerCharacter = Cast<AActionPlayer>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetSectionJumpNotify(this);
	}
}

void UComboAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActionPlayer* OwnerCharacter = Cast<AActionPlayer>(MeshComp->GetOwner());
	if (OwnerCharacter)
	{
		OwnerCharacter->SetSectionJumpNotify(nullptr);
		OwnerCharacter = nullptr;
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
