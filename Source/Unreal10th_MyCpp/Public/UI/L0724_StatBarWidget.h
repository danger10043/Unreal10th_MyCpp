// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L0724_StatBarWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class UNREAL10TH_MYCPP_API UL0724_StatBarWidget : public UUserWidget
{
	GENERATED_BODY()

	// Test Function
public:
	UFUNCTION(BlueprintCallable)
	void TestImageColorChange(FLinearColor InColor);

	UFUNCTION(BlueprintCallable)
	void TestTextChange(FText InString);

	// Canvas Components
protected:
	// Widget 의 컴포넌트는 meta = (BindWidget) 을 넣고, 블루프린트에서의 변수 이름과 동일하게 설정해야 함.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> TestImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TestTextBlock;

	// Transient : 런타임에 일시로 생성
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> TestAnimation;
};
