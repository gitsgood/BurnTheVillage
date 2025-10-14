// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BurnTheVillageDialogue.generated.h"

/**
 * 
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
};

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
};

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