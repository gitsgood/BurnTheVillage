// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BurnTheVillageDialogueWidget.generated.h"

class ABurnTheVillageCharacter;

/**
 * 
 */
UCLASS()
class BURNTHEVILLAGE_API UBurnTheVillageDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
	// Just writing a few Blueprint visible/callable functions to prototype this in blueprints real quick. Might elaborate more on this later.
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dialogue")
	ABurnTheVillageCharacter* PlayerInstance;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	ABurnTheVillageCharacter* GetPlayerCharacterReference(UObject* WorldContextObject);
};
