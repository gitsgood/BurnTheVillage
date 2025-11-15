// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BurnTheVillageInteractInterface.h"	//	Interaction Interface implemented in C++ BECAUSE WE'RE FUCKING CHADS, WE'RE IN IT FOR THE LOVE OF THE GAME, WE BREEATH C++, WE LIVE THIS SHIT
#include "BurnTheVillageNPC.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNPCRecruited, bool, NewValue);

class UWidgetComponent;
class UBurnTheVillageDialogueWidget;

/**
* @brief The character class of the villager NPC's with which we'll be talking with (or not).
*/
UCLASS()
class BURNTHEVILLAGE_API ABurnTheVillageNPC : public ACharacter, public IBurnTheVillageInteractInterface
{
	GENERATED_BODY()

#pragma region Default class code

public:
	// Sets default values for this character's properties
	ABurnTheVillageNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	
#pragma endregion

	//	THIS CODE BELONGS TO US -------------------------------------------------------------------------------------------------------

#pragma region Dialogue

protected:

	/**	
	* @brief The NPCId variable from the dialogue JSONs ought to also be used like their names, they will be the one and the same.
	*	
	* -At this moment, design wise, it's up to us to manually name them to make sure everything works. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	FString NPCId;

	/**
	* @brief The pointer to the existent dialogue widget, nullptr if not in dialogue.
	* 
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	TObjectPtr<UBurnTheVillageDialogueWidget> OngoingDialogueWidgetInstance{ nullptr };

	/** 
	* @brief The class (or subclass) of Widget that we intend to have spawn when dialogue gets initiated.
	* 
	* -It has to be set in the editor, and ideally to the WBP version of the DialogueWidget class.
	* 
	* -This ensures we have the design of the widget we made, while also handling logic in the code.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	TSubclassOf<UBurnTheVillageDialogueWidget> DialogueWidgetClass;

	//Lets us add a texture. To the NPC. So we would get a fun little image in the dialogue like we're cool or something
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	UTexture2D* NPCAvatar;

	//Flags. Crucial for gameplay
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly ,Category = "NPC|Dialogue")
	bool bFinishedDialogue = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly ,Category = "NPC|Dialogue")
	bool bJoinedTheEffort = false;
	
public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FNPCRecruited OnNPCRecruited;
	
	
	
	/**
	* @brief Grants access to this NPC's NPCId string.
	* 
	* @see ABurnTheVillageCharacter::OnDialogueTriggerSphereBeginOverlap for when this gets called.
	*/
	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	FString GetNPCId() const;

	/**
	* @brief Sets the OngoingDialogueWidgetInstance to nullptr.
	* 
	* @see UBurnTheVillageDialogueWidget::DisplayDialogueContent for when this gets used.
	*/
	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	void ClearOngoingDialogueWidgetPointer() { OngoingDialogueWidgetInstance = nullptr; }

	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	UTexture2D* GetNPCAvatar();

	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	bool GetbFinishedDialogue();

	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	bool GetbJoinedTheEffort();

	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	void SetbFinishedDialogue(bool bFinished);

	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	void SetbJoinedTheEffort(bool bJoined);

	

	
	
#pragma endregion

#pragma region Interaction

protected:

	/**
	* @brief Widget component which appears when an NPC is interactable.
	* 
	* -The widget class it contains must be set in the editor.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue|Interaction")
	TObjectPtr<UWidgetComponent> InteractPromptWidgetComponent;

	/**
	* @brief The component non player characters need to have to trigger the interface check.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue|Interaction")
	TObjectPtr<UCapsuleComponent> DialogueTriggerCapsule;

public:

	/**
	* @brief Interface function implementation, sets InteractPromptWidgetComponent to visible if valid interaction collision.
	*/
	UFUNCTION()
	virtual void ShowInteract(AActor* Interactor, bool bIsInRange) override;

	/**
	* @brief Interface function implementation, attempts to trigger dialogue.
	*/
	UFUNCTION()
	virtual void InitiateInteraction(AActor* Interactor) override;

	/**
	* @brief Interface function implementation, sets InteractPromptWidgetComponent visibility to hidden.
	*/
	UFUNCTION()
	virtual void HideInteract(AActor* Interactor) override;

	/**
	* @brief Event triggered when a component of this actor is clicked with the LMB.
	* 
	* -We use this to trigger InitiateInteract by clicking on the NPC's, like you can in Baldur's Gate.
	* 
	* -It is in need of refinement.
	*/
	UFUNCTION()
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

	//	The two functions below were an attempt at implementing NPC highlight on hover.
	//	In Baldur's Gate 3, when the mouse hovers over a talkable NPC, they get highlighted. Hoped to do the same here.
	//	It has not really succeeded yet, might come back to it later.

	//UFUNCTION()
	//virtual void NotifyActorBeginCursorOver() override;

	//UFUNCTION()
	//virtual void NotifyActorEndCursorOver() override;

#pragma endregion
};