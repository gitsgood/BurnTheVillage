// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BurnTheVillageDialogue.h"
#include "BurnTheVillageDialogueManager.generated.h"

/**
* @brief The structure containing the dialogue maps on a per-conversation/NPC basis.
*/
USTRUCT()
struct FDialogueData
{
	//	In UE5, the TMap type is basically an implementation of a hash table data structure.
	//	We use it here in order to lookup the ID's of the next dialogues with an O(1) lookup time.
	//	The pilot code's pointer based implementation was bashing heads with a JSON based implementation.

	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FBurnTheVillageDialogueNode> NodeMap;

	UPROPERTY()
	TMap<FString, FBurnTheVillageDialogueEdge> EdgeMap;
};

/**
* @brief The class that will contain the dialogue content, load it from disk, and handle the state of the dialogue.
* 
* -It is essentialy the dialogue system's backend.
* 
* -It will be instantiated within the player character class, and every class that participates in the dialogue will call its functions through the player class.
*/
UCLASS(BlueprintType)
class BURNTHEVILLAGE_API UBurnTheVillageDialogueManager : public UObject
{
	GENERATED_BODY()
	
protected:
	/**	
	* @brief Below map holds all the different dialogues for all NPC's that could have been loaded from our JSON files.
	* 
	* -The FString key is supposed to be the NPCId found within the JSON.
	*/
	UPROPERTY()
	TMap<FString, FDialogueData> AllDialogues;

#pragma region Conversation State Variables

	/**
	* @brief The key to the AllDialogues map.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue|State")
	FString CurrentNPCId;

	/**
	* @brief This string allows us to keep track of where we are in the dialogue map.
	* 
	* -It is the key to the NodeMap inside the FDialogueData structure.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue|State")
	FString CurrentNodeId;

#pragma endregion

public:
	//	bool return type is used so that later down the line the functions using them can make use of their return to ensure things run if they work and communicate clearly if they don't.

	/**
	* @brief This is the function that finds the JSON, takes in its contents as one huge string, and then populates the FDialogueData struct.
	* 
	* @param[in] FilePath The absolute filepath containing the JSON files we need to build to dialogue content dynamically at runtime.
	* 
	* @see ABurnTheVillageCharacter::BeginPlay for where it is called.
	* @see FBurnTheVillageDialogueGraph for how the structure of the dialogue JSON should look like.
	* 
	* @return True if succesfully loaded dialogue from JSON path, false if it failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	bool LoadDialogueFromFile(const FString& FilePath);

	/**
	* @brief Initiates conversation state by identifying NPC's ID as well as their starting line (Node) ID.
	* 
	* -Verifies the parameters we inputed are valid keys to the dialogue system, and if they are, sets the conversation state variables.
	* 
	* @param[in] NPCId Key to the AllDialogues map.
	* @param[in] StartNodeId Defaults to "START".
	* 
	* @return False if input values are invalid, true if dialogue exists.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	bool InitiateConversationState(const FString& NPCId, const FString& StartNodeId = "START");

	/** 
	* @brief Based on CurrentNPCId, gets a reference to the actual current dialogue Node.
	* 
	* -To use an output parameter function, we're supposed to declared a variable of FBurnTheVillageDialogueNode type before calling this function.
	* 
	* -We can then input our newly declared variable into this and receive it back with the current node.
	* 
	* @param[out] OutNode CurrentNode delivered as an output parameter, assuming it didn't return false before then.
	* 
	* @return Returns false if it cannot get a valid Node, true if it's succesful.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	bool GetCurrentNode(FBurnTheVillageDialogueNode& OutNode);

	/**
	* @brief Gets the full Edges(player dialogue options) based on the CurrentNode.
	* 
	* @return Either an array of the FBurnTheVillageDialogueEdge that the current node directs towards, or an empty array.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	TArray<FBurnTheVillageDialogueEdge> GetCurrentPlayerDialogueOptions();

	/**
	* @brief Takes player's option EdgeId as parameter and sets the CurrentNodeId to the Node that comes next after the Edge the player chose. 
	* 
	* @return Returns false if Next doesn't exist for current Edge. True if it succesfully changed CurrentNodeId to the next one.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	bool AdvanceDialogue(const FString& EdgeId);

	/**
	* @brief Empties CurrentNPCId and CurrentNodeId.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	void EndDialogue();

	/**
	* @brief Returns the actual NPC dialogue content.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	FString GetNPCDialogueContent(const FBurnTheVillageDialogueNode& Node) const;

	/**
	* @brief Returns the actual player dialogue.
	*/
	UFUNCTION(BlueprintCallable, Category = "DialogueManager|Dialogue")
	FString GetPlayerDialogueContent(const FBurnTheVillageDialogueEdge& PlayerEdge) const;
};
