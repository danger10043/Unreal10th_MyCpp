// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/L0724_StatBarWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UL0724_StatBarWidget::TestImageColorChange(FLinearColor InColor)
{
	TestImage->SetColorAndOpacity(InColor);
}

void UL0724_StatBarWidget::TestTextChange(FText InString)
{
	TestTextBlock->SetText(InString);
}
