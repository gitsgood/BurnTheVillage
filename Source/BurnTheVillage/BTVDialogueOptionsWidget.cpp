// Fill out your copyright notice in the Description page of Project Settings.


#include "BTVDialogueOptionsWidget.h"
#include "Components/Button.h"				//	OMG LOOK AT THE BUTTON 
#include "Components/TextBlock.h"			//	HOLY SHIT WE GUESSED THIS LIKE CHADS
#include "BurnTheVillageDialogueWidget.h"	//	Imagine this: every button has a reference to the widget that contains them, and OnDialogueOptionClicked() will trigger a cascade of events that deletes this here widget's instances. Full proof for sure. Safe code. A notation
#include "BurnTheVillageCharacter.h"		//	Needed FOR THE OnDialogueOptionClicked(), INCLUDED BY HUMANS
#include "BurnTheVillageDialogueManager.h"	//	I DID THIS, I NEED THIS FOR THE OnDialogueOptionClicked() function
#include "BTVLoggingControlMacro.h"			//	This header contains ONLY a conditional macro enabling or disabling logging for convenience and eventually performance.

void UBTVDialogueOptionsWidget::OnDialogueOptionClicked()
{
	if (!TheHandThatFedUs.IsValid()) { BTV_LOG(LogTemp, Warning, TEXT("%s says: I do not know who my mother is, I can't fucking call her!"), TEXT(__FUNCTION__)); return; }
	TheHandThatFedUs->AdvanceDisplayedDialogue(OurEdge.EdgeId);
}

void UBTVDialogueOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DialogueOption->OnClicked.AddDynamic(this, &UBTVDialogueOptionsWidget::OnDialogueOptionClicked);
}

void UBTVDialogueOptionsWidget::SetPlayerDialogueText(const FString& InPlayerDialogueContent)
{
	if (!PlayerDialogue)
	{
		BTV_LOG(LogTemp, Warning, TEXT("% s says : PlayerDialogue is not attributed, can not set text..."), TEXT(__FUNCTION__));
		return;
	}
	PlayerDialogue->SetText(FText::FromString(InPlayerDialogueContent));
}
