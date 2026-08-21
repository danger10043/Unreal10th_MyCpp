// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PopupAnim)
	{
		FWidgetAnimationDynamicEvent AnimationFinishedEvent;

		AnimationFinishedEvent.BindDynamic(
			this,
			&UDamageTextWidget::OnDamageAnimationFinished
		);

		BindToAnimationFinished(
			PopupAnim,
			AnimationFinishedEvent
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageTextWidget - PopupAnim 이 할당되지 않았습니다."));
	}
}

void UDamageTextWidget::SetDamageText(float InDamage)
{
	FText DamageTextValue = FText::AsNumber(static_cast<int32>(InDamage));
	DamageText->SetText(DamageTextValue);
}

void UDamageTextWidget::PlayDamageTextAnimation(float InDamage, const FLinearColor& TextColor, const FLinearColor& OutlineColor)
{
	if (!DamageText)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageTextWidget::PlayerDamageTextAnimation - DamageText 가 존재하지 않습니다."));
		return;
	}

	if (!PopupAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDamageTextWidget::PlayerDamageTextAnimation - PopupAnim 가 존재하지 않습니다."));
	}

	SetDamageText(InDamage);

	DamageText->SetColorAndOpacity(FSlateColor(TextColor));
	
	FSlateFontInfo FontInfo = DamageText->GetFont();
	FontInfo.OutlineSettings.OutlineColor = OutlineColor;
	DamageText->SetFont(FontInfo);

	bIsActive = true;
	PlayAnimation(PopupAnim);
}

void UDamageTextWidget::PlayDamageTextAnimationWithoutParam()
{
	if (PopupAnim)
	{
		PlayAnimation(PopupAnim);
	}
}

void UDamageTextWidget::ResetDamageWidget()
{
	bIsActive = false;
	StopAllAnimations();
	SetRenderOpacity(1.0f);
}

void UDamageTextWidget::OnDamageAnimationFinished()
{
	if (bIsActive)
	{
		bIsActive = false;
	}
}