// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "NavigationSystem.h"				//	Matt's A* requires this...
#include "Engine/OverlapResult.h"			//	and this...
#include "PathfinderModule.h"				//	and this one, which he wrote like a chad.
#include "BurnTheVillagePlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * @brief Player controller for a top-down game. In it we bind the interact action enhanced input.
 * 
 * We also intend to implement Matt's A* right here.
 */
UCLASS(abstract)
class ABurnTheVillagePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, Category="Input")
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SetDestinationTouchAction;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** Set to true if we're using touch input */
	uint32 bIsTouch : 1;

	/** Saved location of the character movement destination */
	FVector CachedDestination;

	/** Time that the click input has been pressed */
	float FollowTime = 0.0f;

public:

	/** Constructor */
	ABurnTheVillagePlayerController();

protected:

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;
	
	/** Input handlers */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	//	HUMAN ADDITION IS RIGHT HERE, RIGHT HEEEEEREEE <-----------------------------------------------------------------------------------------------------------------

	/*
	* @brief The interact action. We are still unsure if we want to trigger it with a mouse click or a button press.
	* 
	* We currently support both.
	*/
	UPROPERTY(EditAnywhere, Category = "Input|Interaction")
	TObjectPtr<UInputAction> InteractionAction;

	/**
	* @brief Uses the GetCharacter controller method, casts into our own player character class and then simply calls interaction function from the player.
	*/
	void OnInteractionPressed();

	//	Members live here
public:
	// Instantiate new generation of Pathfinding Gremlin who does ALL the pathfinding
	PathfinderModule PathingGremlin;
	TArray<FVector> NavPoints;
	TArray<FAbstractNodeForNavigation> Graph;

	//	Methods live here
public:
	virtual void BeginPlay() override;
};


