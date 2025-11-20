// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BurnTheVillageCharacter.generated.h"

class UBurnTheVillageDialogueManager;
class ABurnTheVillageNPC;
class UInputAction;
class USphereComponent;

/**
 *  A controllable top-down perspective character
 */
UCLASS(abstract)
class ABurnTheVillageCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Default Template code

private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:

	/** Constructor */
	ABurnTheVillageCharacter();

	/** Initialization */
	virtual void BeginPlay() override;

	/** Update */
	//virtual void Tick(float DeltaSeconds) override;

	/** Returns the camera component **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Returns the Camera Boom component **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

#pragma endregion

	// THIS CODE BELONGS TO US -------------------------------------------------------------------------------------------------------

#pragma region Dialogue

protected:

	/**	@brief  The NPCId of the current valid NPC with dialogue. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Dialogue")
	FString CurrentNPCIdThatIsTalkedTo;

	/**	@brief  The character class will need to instantiate the dialogue manager class in order to handle to dialogue logic. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Dialogue")
	TObjectPtr<UBurnTheVillageDialogueManager>  DialogueManager;

public:

	/**
	* @brief Grants access to encapsulated Dialogue Manager instance instantiated inside the player.
	* 
	* -Any class that intends to affect dialogue in any way would need to declare a pointer of type UBurnTheVillageDialogueManager
	* and assign it using this getter.
	* 
	* @return Returns the adress of the Dialogue Manager class, allowing other classes to call it's functions. .Get() casts into a TObjectPtr explicitly, leaving nothing to chance.
	*/
	UFUNCTION(BlueprintPure, Category = "Player|Dialogue")
	UBurnTheVillageDialogueManager* GetDialogueManager() const;

	/**
	* @brief Sets an FString variable inside the player class.
	* 
	* -It is called every time the player collides with a valid NPC, using that moment to get their NPCId variable and copy it to the player class.
	* 
	* -We do that so that the BurnTheVillageDialogueWidget would only need to get a reference to the player instead of trying to find which NPC we're talking to and getting their reference.
	* 
	* -This is convenient because Unreal provides easy streamlined methods to get a reference to the player, unlike the NPC's.
	* 
	* (In retrospect, this function might have been unecessary if the player class is assigning the variable to itself, but this function is a nice excuse to explain the functionality of the dialogue system in the documentation)
	* 
	* @see OnDialogueTriggerSphereBeginOverlap for information on when this is called
	* 
	* @param[in] CurrentConversationNPCId Passing the interactable NPC's NPCId by const reference.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player|Dialogue")
	void SetCurrentNPCIdThatIsTalkedTo(const FString& CurrentConversationNPCId) { CurrentNPCIdThatIsTalkedTo = CurrentConversationNPCId; }

#pragma endregion

#pragma region Interaction

protected:

	//	Our little trigger sphere triggering the triggers that will trigger the events that trigger the interaction that triggers the dialogue...
	/** @brief  Sphere component from which we will trigger the overlap events allowing interaction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Dialogue|Interaction")
	TObjectPtr<USphereComponent> DialogueTriggerSphere;

private:

	//	These two are here JUST for the button press initiate interact logic. FUCK.

	/** @brief	Internal variable needed for our custom OnInteractionStarted event, which doesn't automatically communicate data to my knowledge. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Dialogue|Interaction", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlappingInteractable = false;

	/**	@brief  OnDialogueTriggerSphereBeginOverlap assigns this to the OtherActor the event delivers. We cast it into BurnTheVillageNPC and retrieve their NPCId this way. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Dialogue|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> CurrentInteractableActor = nullptr;

public:

	/**
	* @brief Returns the player character's stored NPCId.
	* 
	* -To avoid needing a reference to every possible NPC to get their NPCId, this function is conveniently paired with something like GetPlayerCharacterReference from GameplayStatics
	* 
	* @see SetCurrentNPCIdThatIsTalkedTo for information concerning how this is set
	* 
	* @return FString containing current NPCId. Returns an empty string if called without valid NPC.
	*/
	UFUNCTION(BlueprintPure, Category = "Player|Dialogue|Interaction")
	FString GetCurrentNPCIdThatIsTalkedTo() const { return CurrentNPCIdThatIsTalkedTo; }

	/**
	* @brief Returns the pointer to the current OtherActor our player is interacting with.
	* 
	* @see UBurnTheVillageDialogueWidget::DisplayDialogueContent() for when this is useful.
	* 
	* @return CurrentInteractableActor, with a .Get() for explicit conversion to TObjectPtr.
	*/
	UFUNCTION(BlueprintPure, Category = "Player|Dialogue|Interaction")
	AActor* GetCurrentInteractableActor() const { return CurrentInteractableActor.Get(); }

	/**
	* @brief Interface interaction check function.
	* 
	* -This gets triggered by the OnComponentBeginOverlap event.
	* 
	* -The parameters are automatically handed in when the event triggers. In a way, the event expects and needs these parameters to exist in order to function.
	* 
	* -The function triggers the creation of the interact prompt widget by the OtherActor.
	* 
	* -Sets bIsOverlappingInteractable to true and CurrentInteractedWithActor to OtherActor if OtherActor implements the UBurnTheVillageInteractInterface.
	* 
	* -While there are many parameters, by FAR the only one we need is the OtherActor, we may ignore the others for all intents and purposes.
	* 
	* @param[in] OtherActor The actor who contains the overlapped-with mesh.
	*/
	UFUNCTION()
	void OnDialogueTriggerSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/**
	* @brief Post overlap cleanup.
	*
	* -Triggered by the OnComponentEndOverlap event.
	* 
	* -Parameters also dictated by the event itself, we only need OtherActor again.
	* 
	* -Triggers the destruction of the interact prompt widget.
	* 
	* -Cleans up CurrentInteractableActor and sets bIsOverlappingInteractable to false.
	*
	* @param[in] OtherActor The actor who contained the last overlapped-with mesh.
	*/
	UFUNCTION()
	void OnDialogueTriggerSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/**
	* @brief Initiates dialogue interaction event chain.
	* 
	* -Calls the interface's InitiateInteraction implementation if possible.
	* 
	* @see ABurnTheVillageNPC::InitiateInteraction for what follows if Interface is valid.
	*/
	UFUNCTION()
	void OnInteractionStarted();

#pragma endregion
};

