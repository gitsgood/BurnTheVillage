// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BurnTheVillageInteractPrompt.generated.h"

/**
 * 
 */
UCLASS()
class BURNTHEVILLAGE_API UBurnTheVillageInteractPrompt : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractText;

public:
	virtual void NativeConstruct() override;
};
