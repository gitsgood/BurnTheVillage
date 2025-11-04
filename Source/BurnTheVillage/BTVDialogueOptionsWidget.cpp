// Fill out your copyright notice in the Description page of Project Settings.


#include "BTVDialogueOptionsWidget.h"
#include "Components/Button.h"				//	OMG LOOK AT THE BUTTON 
#include "Components/TextBlock.h"			//	HOLY SHIT WE GUESSED THIS LIKE CHADS
#include "BurnTheVillageDialogueWidget.h"	//	Imagine this: every button has a reference to the widget that contains them, and OnDialogueOptionClicked() will trigger a cascade of events that deletes this here widget's instances. Full proof for sure. Safe code. A notation
#include "BurnTheVillageCharacter.h"		//	Needed FOR THE OnDialogueOptionClicked(), INCLUDED BY HUMANS
#include "BurnTheVillageDialogueManager.h"	//	I DID THIS, I NEED THIS FOR THE OnDialogueOptionClicked() function

void UBTVDialogueOptionsWidget::OnDialogueOptionClicked()
{
	if (!TheHandThatFedUs) { UE_LOG(LogTemp, Warning, TEXT("BTVDialogueOptionsWidget says: I do not know who my mother is, I can't fucking call her!")); return; }
	TheHandThatFedUs->AdvanceDisplayedDialogue(OurEdge.EdgeId);
}

void UBTVDialogueOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DialogueOption->OnClicked.AddDynamic(this, &UBTVDialogueOptionsWidget::OnDialogueOptionClicked);
}

void UBTVDialogueOptionsWidget::SetOurEdge(const FBurnTheVillageDialogueEdge& InOurEdge)
{
	OurEdge = InOurEdge;
}

void UBTVDialogueOptionsWidget::SetPlayerDialogueText(FString InPlayerDialogueContent)
{
	PlayerDialogue->SetText(FText::FromString(InPlayerDialogueContent));
}
