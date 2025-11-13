// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageNPC.h"
#include "Components/WidgetComponent.h"		//	NPC's need a pointer to the nice little floating interaction interface.
#include "Components/CapsuleComponent.h"	//	Decided I might as well construct the full thing in here, because why the fuck not.
#include "BurnTheVillageDialogueWidget.h"	//	Turns out the NPC's are responsible for creating the dialogue widget.
#include "BurnTheVillageInteractPrompt.h"	//	Every NPC has their own little pretty interact prompt that shows up.
#include "BTVLoggingControlMacro.h"			//	This header contains ONLY a conditional macro enabling or disabling logging for convenience and eventually performance.

// Sets default values
ABurnTheVillageNPC::ABurnTheVillageNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DialogueTriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DialogueTriggerCapsule"));
	DialogueTriggerCapsule->SetupAttachment(RootComponent);
	DialogueTriggerCapsule->SetWorldScale3D(FVector(3.3875, 3.3875, 3.3875));
	DialogueTriggerCapsule->SetGenerateOverlapEvents(true);

	InteractPromptWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	InteractPromptWidgetComponent->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void ABurnTheVillageNPC::BeginPlay()
{
	Super::BeginPlay();

	if (!InteractPromptWidgetComponent)
	{
		InteractPromptWidgetComponent = NewObject<UWidgetComponent>(this);
		ensure(InteractPromptWidgetComponent);
		InteractPromptWidgetComponent->SetupAttachment(RootComponent);
		BTV_LOG(LogTemp, Warning, TEXT("%s says: Constructor failed to instantiate Widget, I did it instead."), TEXT(__FUNCTION__));
	}

}

#pragma region Dialogue

FString ABurnTheVillageNPC::GetNPCId() const
{
	return NPCId;
}

UTexture2D* ABurnTheVillageNPC::GetNPCAvatar()
{
	return NPCAvatar;
}

bool ABurnTheVillageNPC::GetbFinishedDialogue()
{
	return bFinishedDialogue;
}

bool ABurnTheVillageNPC::GetbJoinedTheEffort()
{
	return bJoinedTheEffort;
}

void ABurnTheVillageNPC::SetbFinishedDialogue(bool bFinished)
{
}

void ABurnTheVillageNPC::SetbJoinedTheEffort(bool bJoined)
{
}


#pragma endregion

#pragma region Interaction

void ABurnTheVillageNPC::ShowInteract(AActor* Interactor, bool bIsInRange)
{
	if (InteractPromptWidgetComponent)
	{
		BTV_LOG(LogTemp, Log, TEXT("%s says: Showing NPC's Interact Prompt widget..."), TEXT(__FUNCTION__));
		InteractPromptWidgetComponent->SetVisibility(bIsInRange);
	}
}

void ABurnTheVillageNPC::InitiateInteraction(AActor* Interactor)
{
	if (!InteractPromptWidgetComponent) return;
	if (!(InteractPromptWidgetComponent->IsVisible())) return;
	if (!DialogueWidgetClass) return;
	if (OngoingDialogueWidgetInstance) return;
	
	OngoingDialogueWidgetInstance = CreateWidget<UBurnTheVillageDialogueWidget>(GetWorld(), DialogueWidgetClass);
	//	The parameter in CreateWidget is asking who has "ownership" over this widget instance. We give it to the world, because "ownership" here simply refers to when the destructor gets called. It will therefore get called when the level is destroyed.
	
	OngoingDialogueWidgetInstance->AddToViewport();	
}

void ABurnTheVillageNPC::HideInteract(AActor* Interactor)
{
	if (InteractPromptWidgetComponent)
	{
		InteractPromptWidgetComponent->SetVisibility(false);
	}
	if (OngoingDialogueWidgetInstance)
	{
		OngoingDialogueWidgetInstance->RemoveFromParent();
		OngoingDialogueWidgetInstance = nullptr;
	}
}

void ABurnTheVillageNPC::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	if (!InteractPromptWidgetComponent)
	{
		return;
	}
	if (!InteractPromptWidgetComponent->IsVisible())
	{
		BTV_LOG(LogTemp, Warning, TEXT("%s says: NPC received click, but interaction can't happen..."), TEXT(__FUNCTION__));
		return;
	}
	BTV_LOG(LogTemp, Log, TEXT("%s says: NPC received click, InitiatingInteraction..."), TEXT(__FUNCTION__));
	InitiateInteraction(this);
}

//void ABurnTheVillageNPC::NotifyActorBeginCursorOver()
//{
//	Super::NotifyActorBeginCursorOver();
//	MY_LOG(LogTemp, Log, TEXT("%s says: NPC detected hover beginning..."), TEXT(__FUNCTION__));
//
//	GetMesh()->SetRenderCustomDepth(true);
//	GetMesh()->SetCustomDepthStencilValue(100);
//}

//void ABurnTheVillageNPC::NotifyActorEndCursorOver()
//{
//	Super::NotifyActorEndCursorOver();
//	MY_LOG(LogTemp, Log, TEXT("%s says: NPC detected hover ending..."), TEXT(__FUNCTION__));
//
//	GetMesh()->SetRenderCustomDepth(false);
//}

#pragma endregion