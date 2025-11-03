// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageDialogueWidget.h"
#include "BurnTheVillageCharacter.h"	//	Character class, obviously
#include "Kismet\GameplayStatics.h"		//	I need a reference to the player character. This has something that can provide that.
#include "BurnTheVillageCharacter.h"			//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BTVDialogueOptionsWidget.h"			//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BurnTheVillageNPC.h"					//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION
#include "BurnTheVillageDialogueManager.h"		//	OUR FUCKING INCLUSIONS MEANT FOR THE DisplayDialogueContent(...) FUNCTION

void UBurnTheVillageDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerInstance = GetPlayerCharacterReference(this);	//	WorldContextObject is surprisingly a smart way to find the player. Anything leads back to it; this class exists in the world, therefore it must lead to it. A good use-case of inheritance I'd say.
}

//	This is a blueprint callable function passing a reference of the player via world context. In blueprints, we may provide it the world context using GetWorld, which we can then link into the little shitter's node.
//	We can then link the function into the actual pointer we have, giving it a reference.
ABurnTheVillageCharacter* UBurnTheVillageDialogueWidget::GetPlayerCharacterReference(const UObject* WorldContextObject) const
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);

	ABurnTheVillageCharacter* PlayerCharacter = Cast<ABurnTheVillageCharacter>(Character);

	if (!PlayerCharacter) { UE_LOG(LogTemp, Warning, TEXT("BurnTheVillageDialogueWidget(...) says: PlayerCharacter reference invalid or cast failed.")); return nullptr; }

	return PlayerCharacter;
}

void UBurnTheVillageDialogueWidget::DisplayDialogueContent(TArray<UBTVDialogueOptionsWidget*> PlayerDialogueOptions, ABurnTheVillageNPC* NPCThatIsBeingDialoguedWith)
{
	UBurnTheVillageDialogueManager* DialogueManager = PlayerInstance->GetDialogueManager();
	if (!DialogueManager) { UE_LOG(LogTemp, Warning, TEXT("DisplayDialogueContent(...) says: Failed to retrieve DialogueManager, aborting...")); return; }
}
