// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BurnTheVillageDialogue.generated.h"

/**
 * @brief This is where we map out part of the JSON's structure.
 * 
 * -More specifically, we specify the Edge object's variable names.
 * 
 * -Currently it only contains the essentials for it to function, however one could expand the dialogue system by adding additional data here.
 */
USTRUCT(BlueprintType)
struct BURNTHEVILLAGE_API FBurnTheVillageDialogueEdge
{
	GENERATED_BODY()

	UPROPERTY()
	FString EdgeId;

	UPROPERTY()
	FString PlayerDialogue;

	UPROPERTY()
	FString NextNodeId;

	//FLAGS BELOW

	 UPROPERTY()
	 bool bFinishedDialogue = false;
	
	 UPROPERTY()
	 bool bJoinedTheEffort = false;
	
	 UPROPERTY()
	 bool bHaveDoubts = false;
	
	 UPROPERTY()
	 bool bVillagerAngered = false;
	
	 UPROPERTY()
	 bool bVillagerKilled = false;

	//FLAGS ABOVE
};

/**
* @brief This is where we map out part of the JSON's structure.
* 
* -More specifically, we specify the Node object's variable names.
* 
* -Currently it only contains the essentials for it to function, however one could expand the dialogue system by adding additional data here.
*/
USTRUCT(BlueprintType)
struct BURNTHEVILLAGE_API FBurnTheVillageDialogueNode
{
	GENERATED_BODY()

	UPROPERTY()
	FString NodeId;

	UPROPERTY()
	FString NPCDialogue;

	UPROPERTY()
	TArray<FString> OutgoingEdgeIds;

	//FLAGS FLAGS EVERYWHERE
	
	UPROPERTY()
	bool bFinishedDialogue = false;
	
	UPROPERTY()
	bool bJoinedTheEffort = false;

	UPROPERTY()
	bool bHaveDoubts = false;

	UPROPERTY()
	bool bVillagerAngered = false;

	UPROPERTY()
	bool bVillagerKilled = false;

	//ALL YOUR FLAGS ARE BELONG TO US
	
};

/**
* @brief This is where we map out the entire JSON's structure.
*
* -JSON natively supports arrays and objects. By defining the objects and placing them as arrays,
* 
* we make it possible to support the dialogue system inside of JSON files.
*
* -The arrays are then turned into a graph navigated through a map when the dialogue manager loads them by using the Id keys we structured the array objects with.
* 
* @see UBurnTheVillageDialogueManager::LoadDialogueFromFile for how the JSON is turned into a graph.
*/
USTRUCT(BlueprintType)
struct BURNTHEVILLAGE_API FBurnTheVillageDialogueGraph
{
	GENERATED_BODY()

	UPROPERTY()
	FString NPCId;

	UPROPERTY()
	TArray<FBurnTheVillageDialogueNode> Nodes;

	UPROPERTY()
	TArray<FBurnTheVillageDialogueEdge> Edges;
};