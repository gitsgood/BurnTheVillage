// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BurnTheVillageCharacter.generated.h"

class UBurnTheVillageDialogueManager;

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

	// The character class will need to instantiate the dialogue manager class in order to handle to dialogue logic.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dialogue")
	UBurnTheVillageDialogueManager* DialogueManager;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UBurnTheVillageDialogueManager* GetDialogueManager() const;
};

