// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class UNREAL10TH_MYCPP_API UDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetDamageText(float InDamage);

	UFUNCTION(BlueprintCallable)
	void PlayDamageTextAnimation(float InDamage, const FLinearColor& TextColor, const FLinearColor& OutlineColor);

	void PlayDamageTextAnimationWithoutParam();

	void ResetDamageWidget();

protected:
	UFUNCTION()
	void OnDamageAnimationFinished();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> PopupAnim = nullptr;

	bool bIsActive = false;
};
