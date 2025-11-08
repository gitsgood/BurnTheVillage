// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageDialogueWidget.h"
#include "BurnTheVillageCharacter.h"			//	Character class, obviously
#include "Kismet\GameplayStatics.h"				//	I need a reference to the player character. This has something that can provide that.
#include "BTVDialogueOptionsWidget.h"			//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BurnTheVillageNPC.h"					//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BurnTheVillageDialogueManager.h"		//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "Components/TextBlock.h"				//	I would like to bind logic into the widget's text blocks from here.
#include "Components/VerticalBox.h"				//	Same as above.
#include "BTVLoggingControlMacro.h"				//	This header contains ONLY a conditional macro enabling or disabling logging for convenience and eventually performance.

#pragma region Dialogue Functionality

void UBurnTheVillageDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerInstance = GetPlayerCharacterReference(this);	//	WorldContextObject is surprisingly a smart way to find the player. Anything leads back to it; this class exists in the world, therefore it must lead to it.
	if (!PlayerInstance.IsValid()) return;

	if (!NPCName) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve NPCName, aborting..."), TEXT(__FUNCTION__)); return; }
	NPCName->SetText(FText::FromString(PlayerInstance->GetCurrentNPCIdThatIsTalkedTo()));
	BTV_LOG(LogTemp, Log, TEXT("%s says: PlayerInstance set to: %s"), TEXT(__FUNCTION__), *PlayerInstance->GetName());

	DialogueManager = PlayerInstance->GetDialogueManager();
	if (!DialogueManager.IsValid()) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve DialogueManager, aborting..."), TEXT(__FUNCTION__)); return; }
	if (!DialogueManager->InitiateConversationState(PlayerInstance->GetCurrentNPCIdThatIsTalkedTo())) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to InitiateDialogueState, aborting..."), TEXT(__FUNCTION__)); return; }
	
	DisplayDialogueContent();
}

ABurnTheVillageCharacter* UBurnTheVillageDialogueWidget::GetPlayerCharacterReference(const UObject* WorldContextObject) const
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);

	ABurnTheVillageCharacter* PlayerCharacter = Cast<ABurnTheVillageCharacter>(Character);

	if (!PlayerCharacter) { BTV_LOG(LogTemp, Warning, TEXT("%s says: PlayerCharacter reference invalid or cast failed."), TEXT(__FUNCTION__)); return nullptr; }

	return PlayerCharacter;
}

void UBurnTheVillageDialogueWidget::DisplayDialogueContent()
{
	if (!DialogueManager.IsValid()) { BTV_LOG(LogTemp, Warning, TEXT("%s says: NativeConstruct failed to retrieve DialogueManager, aborting..."), TEXT(__FUNCTION__)); return; }
	FBurnTheVillageDialogueNode CurrentNPCDialogueNode;
	if (!DialogueManager->GetCurrentNode(CurrentNPCDialogueNode)) 
	{ 
		BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to get CurrentNPCDialogueNode, aborting..."), TEXT(__FUNCTION__));
		this->RemoveFromParent();
		ABurnTheVillageNPC* CurrentNPC = Cast<ABurnTheVillageNPC>(PlayerInstance->GetCurrentInteractableActor());
		if (!CurrentNPC) return;
		CurrentNPC->ClearOngoingDialogueWidgetPointer();
		return; 
	}
	BTV_LOG(LogTemp, Log, TEXT("%s says: CurrentNPCDialogueNode set to %s"), TEXT(__FUNCTION__), *CurrentNPCDialogueNode.NodeId);
	if (!NPCText) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve NPCText, aborting..."), TEXT(__FUNCTION__)); return; }
	NPCText->SetText(FText::FromString(DialogueManager->GetNPCDialogueContent(CurrentNPCDialogueNode)));

	if(!PlayerDialogueOptionContainer){
		BTV_LOG(LogTemp, Warning, TEXT("%s says: PlayerDialogueOptionContainer is null, aborting..."), TEXT(__FUNCTION__));
		return;
	}

	BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Assigning Player Edge array..."), TEXT(__FUNCTION__));
	TArray<FBurnTheVillageDialogueEdge> CurrentPlayerDialogueEdges = DialogueManager->GetCurrentPlayerDialogueOptions();

	//	Arbitrarily limiting the scope of the for loop just so that the IterationCount variable dies just that little bit sooner.
	{
		BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Player Edge array succesfully assigned, attempting the for loop..."), TEXT(__FUNCTION__));
		uint8 IterationCount{ 0 };
		for (const FBurnTheVillageDialogueEdge& Edge : CurrentPlayerDialogueEdges)
		{
			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Attempting to Create Widget..."), TEXT(__FUNCTION__));
			UBTVDialogueOptionsWidget* DialogueOption = CreateWidget<UBTVDialogueOptionsWidget>(this, DialogueOptionsWidget);

			if (!DialogueOption) { BTV_VERBOSE_LOG(LogTemp, Warning, TEXT("%s says: Failed to create UBTVDialogueOptionsWidget, aborting..."), TEXT(__FUNCTION__)); break; }

			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Attempting to add widget to widget array..."), TEXT(__FUNCTION__));
			CurrentPlayerOptions.Add(DialogueOption);

			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Attempting to set the text of the newly formed widget..."), TEXT(__FUNCTION__));
			DialogueOption->SetPlayerDialogueText(DialogueManager->GetPlayerDialogueContent(Edge));

			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Attempting to set hand that fed us..."), TEXT(__FUNCTION__));
			DialogueOption->SetTheHandThatFedUs(this);

			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Attempting to assign edge to baby..."), TEXT(__FUNCTION__));
			DialogueOption->SetOurEdge(Edge);

			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Attemtping to add the baby to the vertical box..."), TEXT(__FUNCTION__));
			PlayerDialogueOptionContainer->AddChildToVerticalBox(DialogueOption);

			IterationCount++;
			BTV_VERBOSE_LOG(LogTemp, Log, TEXT("%s says: Iterated the loop %d out of %d times, continuing..."), TEXT(__FUNCTION__), IterationCount, CurrentPlayerDialogueEdges.Num());
		}
	}
}

void UBurnTheVillageDialogueWidget::ClearPlayerOptions()
{
	for (const TObjectPtr<UBTVDialogueOptionsWidget>& DialogueOptionWidget : CurrentPlayerOptions)
	{
		if (!DialogueOptionWidget) continue;
		DialogueOptionWidget->RemoveFromParent();
	}
	CurrentPlayerOptions.Empty();
}

void UBurnTheVillageDialogueWidget::AdvanceDisplayedDialogue(const FString ChosenEdgeId)
{
	DialogueManager->AdvanceDialogue(ChosenEdgeId);
	ClearPlayerOptions();
	DisplayDialogueContent();
}

#pragma endregion