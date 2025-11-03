// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BurnTheVillageCharacter.generated.h"

class UBurnTheVillageDialogueManager;
class ABurnTheVillageNPC;
class UInputAction;

/**
 *  A controllable top-down perspective character
 */
UCLASS(abstract)
class ABurnTheVillageCharacter : public ACharacter
{
	GENERATED_BODY()

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
	virtual void Tick(float DeltaSeconds) override;

	/** Returns the camera component **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Returns the Camera Boom component **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// THIS CODE BELONGS TO US -------------------------------------------------------------------------------------------------------
protected:
	//	Our little trigger sphere triggering the triggers that will trigger the events that trigger the interaction that triggers the dialogue...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	class USphereComponent* SphereTrigger;

private:
	//	These two are here JUST for the button press initiate interact logic. FUCK.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	bool bIsOverlappingInteractable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	AActor* CurrentInteractableActor = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Dialogue")
	FString CurrentNPCIDThatIsTalkedTo;

	// The character class will need to instantiate the dialogue manager class in order to handle to dialogue logic.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Dialogue")
	UBurnTheVillageDialogueManager* DialogueManager;

public:
	UFUNCTION(BlueprintCallable, Category = "Player|Dialogue")
	UBurnTheVillageDialogueManager* GetDialogueManager() const;

	UFUNCTION(BlueprintCallable, Category = "Player|Dialogue")
	void SetCurrentNPCIDThatIsTalkedTo(FString CurrentConversationNPC) { CurrentNPCIDThatIsTalkedTo = CurrentConversationNPC; }

	UFUNCTION()
	void OnTriggerSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnTriggerSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION()
	void OnInteractionStarted();
};

