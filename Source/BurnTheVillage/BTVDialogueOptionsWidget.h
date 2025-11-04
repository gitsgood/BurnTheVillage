// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BurnTheVillageDialogue.h"						//	I swear I didnt want to do this, but you cant forward declare structs...
#include "BTVDialogueOptionsWidget.generated.h"

class UButton;
class UTextBlock;
class UBurnTheVillageDialogueWidget;

/**
 * 
 */
UCLASS()
class BURNTHEVILLAGE_API UBTVDialogueOptionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* DialogueOption;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerDialogue;

	UPROPERTY()
	UBurnTheVillageDialogueWidget* TheHandThatFedUs;

	UPROPERTY()
	FBurnTheVillageDialogueEdge OurEdge;

	UFUNCTION()
	void OnDialogueOptionClicked();

public:
	virtual void NativeConstruct() override;

	//	This will be called by the actual mother at the moment of birth, letting her little babies find their way back into the womb.
	UFUNCTION(BlueprintCallable, Category = "BTVDialogueOptions|Dialogue")
	void SetTheHandThatFedUs(UBurnTheVillageDialogueWidget* InOurMother) { TheHandThatFedUs = InOurMother; }

	UFUNCTION(BlueprintCallable, Category = "BTVDialogueOptions|Dialogue")
	void SetOurEdge(const FBurnTheVillageDialogueEdge& InOurEdge);

	UFUNCTION(BlueprintCallable, Category = "BTVDialogueOptions|Dialogue")
	void SetPlayerDialogueText(FString InPlayerDialogueContent);
};
