// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageDialogueWidget.h"
#include "BurnTheVillageCharacter.h"	//	Character class, obviously
#include "Kismet\GameplayStatics.h"		//	I need a reference to the player character. This has something that can provide that.

//	This is a blueprint callable function passing a reference of the player via world context. In blueprints, we may provide it the world context using GetWorld, which we can then link into the little shitter's node.
//	We can then link the function into the actual pointer we have, giving it a reference.
ABurnTheVillageCharacter* UBurnTheVillageDialogueWidget::GetPlayerCharacterReference(const UObject* WorldContextObject) const
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);

	ABurnTheVillageCharacter* PlayerCharacter = Cast<ABurnTheVillageCharacter>(Character);

	if (!PlayerCharacter) { UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter reference invalid or cast failed.")); }

	return PlayerCharacter;
}
