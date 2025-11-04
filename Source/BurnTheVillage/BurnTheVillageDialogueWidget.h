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
 * 
 */
UCLASS()
class BURNTHEVILLAGE_API UBurnTheVillageDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
	//	Just writing a few Blueprint visible/callable functions to prototype this in blueprints real quick. Might elaborate more on this later.
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	TArray<UBTVDialogueOptionsWidget*> CurrentPlayerOptions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	ABurnTheVillageCharacter* PlayerInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	UBurnTheVillageDialogueManager* DialogueManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DialogueWidget|Dialogue")
	TSubclassOf<UBTVDialogueOptionsWidget> DialogueOptionsWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NPCText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NPCName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* PlayerDialogueOptionContainer;

public:
	//	At this moment in time, we're moving the blueprint nonsense into real, hard, code.
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	ABurnTheVillageCharacter* GetPlayerCharacterReference(const UObject* WorldContextObject) const;

	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	void DisplayDialogueContent();

	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	void ClearPlayerOptions();

	UFUNCTION(BlueprintCallable, Category = "DialogueWidget|Dialogue")
	void AdvanceDisplayedDialogue(const FString ChosenEdgeId);
};
