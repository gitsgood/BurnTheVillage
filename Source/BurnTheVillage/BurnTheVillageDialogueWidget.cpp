// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageDialogueWidget.h"
#include "BurnTheVillageCharacter.h"			//	Character class, obviously
#include "Kismet\GameplayStatics.h"				//	I need a reference to the player character. This has something that can provide that.
#include "BurnTheVillageCharacter.h"			//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BTVDialogueOptionsWidget.h"			//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BurnTheVillageNPC.h"					//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BurnTheVillageDialogueManager.h"		//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "Components/TextBlock.h"				//	I would like to bind logic into the widget's text blocks from here.
#include "Components/VerticalBox.h"				//	Same as above.

void UBurnTheVillageDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerInstance = GetPlayerCharacterReference(this);	//	WorldContextObject is surprisingly a smart way to find the player. Anything leads back to it; this class exists in the world, therefore it must lead to it. A good use-case of inheritance I'd say.
	if (!PlayerInstance) return;
	if (!NPCName) { UE_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve NPCName, aborting..."), TEXT(__FUNCTION__)); return; }
	NPCName->SetText(FText::FromString(PlayerInstance->GetCurrentNPCIdThatIsTalkedTo()));
	UE_LOG(LogTemp, Log, TEXT("%s says: PlayerInstance set to: %s"), TEXT(__FUNCTION__), *PlayerInstance->GetName());
	DialogueManager = PlayerInstance->GetDialogueManager();
	if (!DialogueManager) { UE_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve DialogueManager, aborting..."), TEXT(__FUNCTION__)); return; }
	if (!DialogueManager->InitiateConversationState(PlayerInstance->GetCurrentNPCIdThatIsTalkedTo())) { UE_LOG(LogTemp, Warning, TEXT("%s says: Failed to InitiateDialogueState, aborting..."), TEXT(__FUNCTION__)); return; }
	
	DisplayDialogueContent();
}

//	This is a blueprint callable function passing a reference of the player via world context. In blueprints, we may provide it the world context using GetWorld, which we can then link into the little shitter's node.
//	We can then link the function into the actual pointer we have, giving it a reference.
ABurnTheVillageCharacter* UBurnTheVillageDialogueWidget::GetPlayerCharacterReference(const UObject* WorldContextObject) const
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);

	ABurnTheVillageCharacter* PlayerCharacter = Cast<ABurnTheVillageCharacter>(Character);

	if (!PlayerCharacter) { UE_LOG(LogTemp, Warning, TEXT("%s says: PlayerCharacter reference invalid or cast failed."), TEXT(__FUNCTION__)); return nullptr; }

	return PlayerCharacter;
}

void UBurnTheVillageDialogueWidget::DisplayDialogueContent()
{
	if (!DialogueManager) { UE_LOG(LogTemp, Warning, TEXT("%s says: NativeConstruct to retrieve DialogueManager, aborting..."), TEXT(__FUNCTION__)); return; }
	FBurnTheVillageDialogueNode CurrentNPCDialogueNode;
	if (!DialogueManager->GetCurrentNode(CurrentNPCDialogueNode)) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("%s says: Failed to get CurrentNPCDialogueNode, aborting..."), TEXT(__FUNCTION__)); 
		this->RemoveFromParent();
		ABurnTheVillageNPC* CurrentNPC = Cast<ABurnTheVillageNPC>(PlayerInstance->GetCurrentInteractableActor());
		if (!CurrentNPC) return;
		CurrentNPC->ClearOngoingDialogueWidgetPointer();
		return; 
	}
	UE_LOG(LogTemp, Log, TEXT("%s says: CurrentNPCDialogueNode set to %s"), TEXT(__FUNCTION__), *CurrentNPCDialogueNode.NodeId);
	if (!NPCText) { UE_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve NPCText, aborting..."), TEXT(__FUNCTION__)); return; }
	NPCText->SetText(FText::FromString(DialogueManager->GetNPCDialogueContent(CurrentNPCDialogueNode)));

	if(!PlayerDialogueOptionContainer){
		UE_LOG(LogTemp, Warning, TEXT("%s says: PlayerDialogueOptionContainer is null, aborting..."), TEXT(__FUNCTION__));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s says: Assigning Player Edge array..."), TEXT(__FUNCTION__));
	TArray<FBurnTheVillageDialogueEdge> CurrentPlayerDialogueEdge = DialogueManager->GetCurrentPlayerDialogueOptions();
	UE_LOG(LogTemp, Log, TEXT("%s says: Player Edge array succesfully assigned, attempting the for loop..."), TEXT(__FUNCTION__));
	for (const FBurnTheVillageDialogueEdge& Edge : CurrentPlayerDialogueEdge)
	{
		UE_LOG(LogTemp, Log, TEXT("%s says: Attempting to Create Widget..."), TEXT(__FUNCTION__));
		UBTVDialogueOptionsWidget* DialogueOption = CreateWidget<UBTVDialogueOptionsWidget>(this, DialogueOptionsWidget);

		if(!DialogueOption) { UE_LOG(LogTemp, Warning, TEXT("%s says: Failed to create UBTVDialogueOptionsWidget, aborting..."), TEXT(__FUNCTION__)); break; }

		UE_LOG(LogTemp, Log, TEXT("%s says: Attempting to add widget to widget array..."), TEXT(__FUNCTION__));
		CurrentPlayerOptions.Add(DialogueOption);

		UE_LOG(LogTemp, Log, TEXT("%s says: Attempting to set the text of the newly formed widget..."), TEXT(__FUNCTION__));
		DialogueOption->SetPlayerDialogueText(DialogueManager->GetPlayerDialogueContent(Edge));

		UE_LOG(LogTemp, Log, TEXT("%s says: Attempting to set hand that fed us..."), TEXT(__FUNCTION__));
		DialogueOption->SetTheHandThatFedUs(this);

		UE_LOG(LogTemp, Log, TEXT("%s says: Attempting to assign edge to baby..."), TEXT(__FUNCTION__));
		DialogueOption->SetOurEdge(Edge);

		UE_LOG(LogTemp, Log, TEXT("%s says: Attemtping to add the baby to the vertical box..."), TEXT(__FUNCTION__));
		PlayerDialogueOptionContainer->AddChildToVerticalBox(DialogueOption);

		UE_LOG(LogTemp, Log, TEXT("%s says: Attempting the loop, or exit..."), TEXT(__FUNCTION__));
	}
}

void UBurnTheVillageDialogueWidget::ClearPlayerOptions()
{
	for (UBTVDialogueOptionsWidget* DialogueOptionWidget : CurrentPlayerOptions)
	{
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
