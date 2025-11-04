// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageNPC.h"
#include "Components/WidgetComponent.h"		//	NPC's need a pointer to the nice little floating interaction interface.
#include "Components/CapsuleComponent.h"	//	Decided I might as well construct the full thing in here, because why the fuck not.
#include "BurnTheVillageDialogueWidget.h"	//	Turns out the NPC's are responsible for creating the dialogue widget.
#include "BurnTheVillageInteractPrompt.h"	//	Every NPC has their own little pretty interact prompt that shows up.

// Sets default values
ABurnTheVillageNPC::ABurnTheVillageNPC()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleTrigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleTrigger"));
	CapsuleTrigger->SetupAttachment(RootComponent);
	CapsuleTrigger->SetWorldScale3D(FVector(3.3875, 3.3875, 3.3875));
	CapsuleTrigger->SetGenerateOverlapEvents(true);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void ABurnTheVillageNPC::BeginPlay()
{
	Super::BeginPlay();

	if (!Widget)
	{
		Widget = NewObject<UWidgetComponent>(this);
		ensure(Widget);
		Widget->SetupAttachment(RootComponent);
		UE_LOG(LogTemp, Log, TEXT("%s says: Constructor failed to instantiate Widget, I did it instead."), TEXT(__FUNCTION__));
	}

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
	{
		UE_LOG(LogTemp, Log, TEXT("%s says: Showing NPC's Interact Prompt widget..."), TEXT(__FUNCTION__));
		Widget->SetVisibility(bIsInRange);
	}
}

void ABurnTheVillageNPC::InitiateInteraction(AActor* Interactor)
{
	if (!Widget) return;

	if (!(Widget->IsVisible())) return;

	if (!DialogueWidgetClass) return;
	if (OngoingDialogueWidgetInstance) return;
	OngoingDialogueWidgetInstance = CreateWidget<UBurnTheVillageDialogueWidget>(GetWorld(), DialogueWidgetClass);
	OngoingDialogueWidgetInstance->AddToViewport();	//	The parameter in CreateWidget is asking who has "ownership" over this widget instance. We give it to the world, because "ownership" here simply refers to when the destructor gets called. It will therefore get called when the level is destroyed.
}

void ABurnTheVillageNPC::HideInteract(AActor* Interactor)
{
	if (Widget)
	{
		Widget->SetVisibility(false);
	}
	if (OngoingDialogueWidgetInstance)
	{
		OngoingDialogueWidgetInstance->RemoveFromParent();
		OngoingDialogueWidgetInstance = nullptr;
	}
}

void ABurnTheVillageNPC::NotifyActorOnClicked(FKey ButtonPressed)
{
	if (!Widget) 
	{
		UE_LOG(LogTemp, Log, TEXT("%s says: NPC received click, but interaction can't happen..."), TEXT(__FUNCTION__));
		return;
	}
	Super::NotifyActorOnClicked(ButtonPressed);
	InitiateInteraction(this);
}

//void ABurnTheVillageNPC::NotifyActorBeginCursorOver()
//{
//	Super::NotifyActorBeginCursorOver();
//	UE_LOG(LogTemp, Log, TEXT("%s says: NPC detected hover beginning..."), TEXT(__FUNCTION__));
//
//	GetMesh()->SetRenderCustomDepth(true);
//	GetMesh()->SetCustomDepthStencilValue(100);
//}
//
//void ABurnTheVillageNPC::NotifyActorEndCursorOver()
//{
//	Super::NotifyActorEndCursorOver();
//	UE_LOG(LogTemp, Log, TEXT("%s says: NPC detected hover ending..."), TEXT(__FUNCTION__));
//
//	GetMesh()->SetRenderCustomDepth(false);
//}

