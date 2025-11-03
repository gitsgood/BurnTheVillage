// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageNPC.h"
#include "Components/WidgetComponent.h"		//	NPC's need a pointer to the nice little floating interaction interface.
#include "Components/CapsuleComponent.h"	//	Decided I might as well construct the full thing in here, because why the fuck not.
#include "BurnTheVillageDialogueWidget.h"	//	Turns out the NPC's are responsible for creating the dialogue widget.

// Sets default values
ABurnTheVillageNPC::ABurnTheVillageNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleTrigger"));
	CapsuleTrigger->SetupAttachment(RootComponent);
	CapsuleTrigger->SetWorldScale3D(FVector(3.3875, 3.3875, 3.3875));
	CapsuleTrigger->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void ABurnTheVillageNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABurnTheVillageNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABurnTheVillageNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FString ABurnTheVillageNPC::GetNPCId() const
{
	return NPCId;
}

void ABurnTheVillageNPC::ShowInteract(AActor* Interactor, bool bIsInRange)
{
	if (Widget)
		Widget->SetHiddenInGame(!bIsInRange);
}

void ABurnTheVillageNPC::InitiateInteraction(AActor* Interactor)
{
	if (!Widget) return;

	if (!(Widget->IsVisible())) return;

	CreateWidget<UBurnTheVillageDialogueWidget>(GetWorld())->AddToViewport();	//	The parameter in CreateWidget is asking who has "ownership" over this widget instance. We give it to the world, because "ownership" here simply refers to when the destructor gets called. It will therefore get called when the level is destroyed.
}

void ABurnTheVillageNPC::HideInteract(AActor* Interactor)
{
	if (Widget)
		Widget->SetHiddenInGame(true);
}

