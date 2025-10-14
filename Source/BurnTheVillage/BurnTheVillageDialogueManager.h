// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BurnTheVillageDialogue.h"
#include "BurnTheVillageDialogueManager.generated.h"

//	Below struct is assigned on a per NPC basis, depending on whether or not they hold their own NPCId.
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

UCLASS()
class BURNTHEVILLAGE_API UBurnTheVillageDialogueManager : public UObject
{
	GENERATED_BODY()
	
protected:
	//	Below map holds all the different dialogues for all NPC's that could have been loaded from our JSON files.
	UPROPERTY()
	TMap<FString, FDialogueData> AllDialogues;

#pragma region Conversation State Variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue|State")
	FString CurrentNPCId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue|State")
	FString CurrentNodeId;

#pragma endregion

public:
	//	bool return type is used so that later down the line the functions using them can make use of their return to ensure things run if they work and communicate clearly if they don't.

	//	This is the function that finds the JSON, takes in its contents as one huge string, and then populates the FDialogueData struct.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool LoadDialogueFromFile(const FString& FilePath);

	//	Initiates conversation state by identifying NPC's ID as well as their starting line (Node) ID.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool InitiateConversationState(const FString& NPCId, const FString& StartNodeId = "START");

	//	Based on CurrentNPCId, gets a pointer to the actual current dialogue Node.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool GetCurrentNode(FBurnTheVillageDialogueNode& OutNode);

	//	Gets the full Edges (player dialogue options) the CurrentNode is connected to.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	TArray<FBurnTheVillageDialogueEdge> GetCurrentPlayerDialogueOptions();

	//	Takes player's option EdgeId as parameter and sets the CurrentNodeId to the Node that comes next after the Edge the player chose. Returns false if Next doesn't exist for current Edge.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool AdvanceDialogue(const FString& EdgeId);

	//	Empties CurrentNPCId and CurrentNodeId.
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();
};
