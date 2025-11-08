// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BurnTheVillageDialogueWidget.generated.h"

class ABurnTheVillageCharacter;
class UBTVDialogueOptionsWidget;
class ABurnTheVillageCharacter;
class ABurnTheVillageNPC;
class UBurnTheVillageDialogueManager;

class UTextBlock;
class UVerticalBox;

/**
 * @brief Widget handling the front end of the dialogue system. It's what shows it.
 */
UCLASS()
class BURNTHEVILLAGE_API UBurnTheVillageDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Dialogue Components

protected:

	/**
	* @brief TextBlock component that would hold the NPC's dialogue.
	* 
	* -If they don't have dialogue, it will show the blueprint's default text.
	*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> NPCText;

	/**
	* @brief TextBlock component that would hold the NPC's name.
	* 
	* -It currently get its text from the NPC's NPCId attribute.
	* 
	* TODO: It might help with scalability to add a new FString variable inside the NPC class exclusively for the name. This way NPCId's can be unique despite same names. 
	* 
	* @see UBurnTheVillageDialogueWidget::NativeConstruct is where its value is set.
	*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> NPCName;

	/**
	* @brief VerticalBox component that would contain a dynamic amount of UBTVDialogueOptionsWidget
	* 
	* -Using a vertical box here is pretty clever, as it automatically takes care of the placement of its children once it is set-up.
	* (Rain probably already knew that, seing that she was the one to design this) ;)
	*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerDialogueOptionContainer;

#pragma endregion

#pragma region Dialogue Functionality

protected:

	/**
	* @brief Array keeping track of the UBTVDialogueOptionsWidget inside the PlayerDialogueOptionContainer.
	* 
	* @see UBurnTheVillageDialogueWidget::DisplayDialogueContent for when this gets populated.
	* 
	* @see UBurnTheVillageDialogueWidget::ClearPlayerOptions for when this gets cleared.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	TArray<TObjectPtr<UBTVDialogueOptionsWidget>> CurrentPlayerOptions;

	/**
	* @brief A reference to the player.
	* 
	* -It uses TWeakObjectPtr as the widget has no ownership over the player character object.
	* This means the player object does not get destructed when this widget goes out of scope.
	* 
	* @see UBurnTheVillageDialogueWidget::NativeConstruct for when this is assigned.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	TWeakObjectPtr<ABurnTheVillageCharacter> PlayerInstance;

	/**
	* @brief A reference to the player's dialogue manager.
	* 
	* -It uses TWeakObjectPtr as the widget has no ownership over the dialogue manager, the player does.
	* 
	* @see UBurnTheVillageDialogueWidget::NativeConstruct for when this is assigned.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	TWeakObjectPtr<UBurnTheVillageDialogueManager> DialogueManager;

	/**
	* @brief Variable intended to be set in the editor into the blueprint child of UBTVDialogueOptionsWidget.
	* 
	* -When creating widgets, it is a very good idea to have a subclass variable so that we can design it in a blueprint but still use logic written in code.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	TSubclassOf<UBTVDialogueOptionsWidget> DialogueOptionsWidget;

public:

	/**
	* @brief Called when this widget is created.
	* We use it to initialise our pointers and call DisplayDialogueContent immediately.
	*/
	virtual void NativeConstruct() override;

	/**
	* @brief This is a blueprint callable function passing a reference of the player via world context. 
	* In blueprints, we may provide it the world context using GetWorld, which we can then link into the little shitter's node. 
	* We can then link the function into the actual pointer we have, giving it a reference.
	* 
	* @param[in] WorldContextObject Can be anything that exists in the level. In code we can just use "this" keyword and let it cook.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	ABurnTheVillageCharacter* GetPlayerCharacterReference(const UObject* WorldContextObject) const;

	/**
	* @brief Calls the player's DialogueManager in order to retrieve and display dialogue content.
	* 
	* -Will automatically end dialogue if the dialogue option you select has no follow up.
	* 
	* -Will otherwise set the text of the NPCText variable as well as create the player dialogue option widgets.
	* 
	* @see UBurnTheVillageDialogueManager::GetCurrentNode for how the condition to end dialogue or not works.
	* 
	* @see UBTVDialogueOptionsWidget for how the DialogueOptionsWidget is set up.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	void DisplayDialogueContent();

	/**
	* @brief Iterates through the CurrentPlayerOptions array and removes every single DialogueOptionsWidget from the PlayerDialogueOptionContainer.
	* 
	* -It then empties the array, destroying its pointers and preparing it for the next cycle.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	void ClearPlayerOptions();

	/**
	* @brief Advances dialogue state in the manager and calls ClearPlayerOptions and then DisplayDialogueContent.
	* 
	* @see UBurnTheVillageDialogueManager::AdvanceDialogue for what happens inside the manager.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	void AdvanceDisplayedDialogue(const FString ChosenEdgeId);

#pragma endregion
};
