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
 * @brief Widget containing the player dialogue text and button functionality inside of the main dialogue widget.
 */
UCLASS()
class BURNTHEVILLAGE_API UBTVDialogueOptionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	/**
	* @brief Button calling the event that triggers dialogue advancement.
	* 
	* @see UBTVDialogueOptionsWidget::OnDialogueOptionClicked for what it's OnClicked event does.
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DialogueOption;

	/**
	* @brief Text block inside of the DialogueOption button displaying the actual dialogue option content.
	* 
	* @see UBTVDialogueOptionsWidget::SetPlayerDialogueText for how it's set.
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerDialogue;

	/**
	* @brief Non owning pointer to the main dialogue widget.
	* 
	* -We use this reference to call the function that advances the dialogue (and also removes this widget and all others).
	* 
	* @see UBurnTheVillageDialogueWidget::DisplayDialogueContent to see when this is initialised.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BTVDialogueOptions|Dialogue")
	TWeakObjectPtr<UBurnTheVillageDialogueWidget> TheHandThatFedUs;

	/**
	* @brief The FBurnTheVillageDialogueEdge structure containing all the information the dialogue system needs.
	* 
	* @see UBTVDialogueOptionsWidget::OnDialogueOptionClicked for where the EdgeId is fed into the dialogue manager.
	* 
	* @see UBurnTheVillageDialogueWidget::DisplayDialogueContent for when this is initialised.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BTVDialogueOptions|Dialogue")
	FBurnTheVillageDialogueEdge OurEdge;

	/**
	* @brief Calls the main dialogue widget's function to trigger dialogue advancement.
	* 
	* @see UBurnTheVillageDialogueWidget::AdvanceDisplayedDialogue to see what the function it calls does.
	*/
	UFUNCTION()
	void OnDialogueOptionClicked();

public:

	/**
	* @brief We use this simply to bind the OnClicked event.
	* 
	* @see UBTVDialogueOptionsWidget::OnDialogueOptionClicked for which function we bound.
	*/
	virtual void NativeConstruct() override;

	/**	
	* @brief This will be called by the actual mother at the moment of birth, letting her little babies find their way back into the womb.
	* 
	* -Feeds a reference of the main dialogue widget to TheHandThatFedUs.
	* 
	* @param[in] InOurMother Pointer to the main dialogue widget. Called using "this" keyword by UBurnTheVillageDialogueWidget.
	*/
	UFUNCTION(BlueprintCallable, Category = "BTVDialogueOptions|Dialogue")
	void SetTheHandThatFedUs(UBurnTheVillageDialogueWidget* InOurMother) { TheHandThatFedUs = InOurMother; }

	/**
	* @brief Sets OurEdge variable.
	*/
	UFUNCTION(BlueprintCallable, Category = "BTVDialogueOptions|Dialogue")
	void SetOurEdge(const FBurnTheVillageDialogueEdge& InOurEdge) { OurEdge = InOurEdge; }

	/**
	* @brief Sets PlayerDialogue text content.
	*/
	UFUNCTION(BlueprintCallable, Category = "BTVDialogueOptions|Dialogue")
	void SetPlayerDialogueText(const FString& InPlayerDialogueContent);
};
