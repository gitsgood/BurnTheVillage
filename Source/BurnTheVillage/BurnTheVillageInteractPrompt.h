// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BurnTheVillageInteractPrompt.generated.h"

/**
 * @brief Parent widget of the interact prompt widget.
 * 
 * -The class is currently unecessary, however keeping it around might come in handy eventually.
 * 
 * @see ABurnTheVillageNPC::InteractPromptWidgetComponent for where this is used.
 */
UCLASS()
class BURNTHEVILLAGE_API UBurnTheVillageInteractPrompt : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractText;

public:
	virtual void NativeConstruct() override;
};
