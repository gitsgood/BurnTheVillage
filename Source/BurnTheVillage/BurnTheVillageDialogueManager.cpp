// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageDialogueManager.h"
#include "Misc/FileHelper.h"
#include "JsonObjectConverter.h"
#include "BurnTheVillageDialogue.h"

bool UBurnTheVillageDialogueManager::LoadDialogueFromFile(const FString& FilePath)
{
	//	We read the file, and take its contents as a string into JsonString.
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) { UE_LOG(LogTemp, Warning, TEXT("LoadDialogueFromFile(...): Filepath not found: %s"), *FilePath); return false; }

	//	Here, we do the deserialisation: we convert the JsonString into a FBurnTheVillageDialogueGraph type.
	FBurnTheVillageDialogueGraph LoadedDialogueGraph;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &LoadedDialogueGraph, 0, 0)) { UE_LOG(LogTemp, Warning, TEXT("LoadDialogueFromFile(...): JSON deserialisation failed")); return false; }

	//	If the dialogue data has no NPC identification, we do not let it cross this border.
	const FString& NPCIdFromFile = LoadedDialogueGraph.NPCId;
	if (NPCIdFromFile.IsEmpty()) { UE_LOG(LogTemp, Warning, TEXT("LoadDialogueFromFile(...): NPCId is empty, aborting")); return false; }

	//	We now take the edges from the list graph into our hash table graph, including the Id's as keys.
	FDialogueData NewDialogueData;
	for (const FBurnTheVillageDialogueNode& Node : LoadedDialogueGraph.Nodes)
	{
		NewDialogueData.NodeMap.Add(Node.NodeId, Node);
	}
	for (const FBurnTheVillageDialogueEdge& Edge : LoadedDialogueGraph.Edges)
	{
		NewDialogueData.EdgeMap.Add(Edge.EdgeId, Edge);
	}

	//	We add the newly populated hash table graph into our dialogue hash table, including the NPCId as its unique key.
	AllDialogues.Add(NPCIdFromFile, NewDialogueData);
	UE_LOG(LogTemp, Log, TEXT("LoadDialogueFromFile(): Succesfully loaded dialogue for NPC '%s' with %d nodes and %d edges."), *NPCIdFromFile, NewDialogueData.NodeMap.Num(), NewDialogueData.EdgeMap.Num());
	return true;
}

bool UBurnTheVillageDialogueManager::InitiateConversationState(const FString& NPCId, const FString& StartNodeId)
{
	const FDialogueData* DialogueData = AllDialogues.Find(NPCId);
	if (!DialogueData) { UE_LOG(LogTemp, Warning, TEXT("StartDialogue(...): No dialogue found for this NPCId: %s"), *NPCId); return false; }

	if (!DialogueData->NodeMap.Contains(StartNodeId)) { UE_LOG(LogTemp, Warning, TEXT("StartDialogue(...): StartingNode %s not found for NPCId %s"), *StartNodeId, *NPCId); return false; }

	CurrentNPCId = NPCId;
	CurrentNodeId = StartNodeId;
	return true;
}

bool UBurnTheVillageDialogueManager::GetCurrentNode(FBurnTheVillageDialogueNode& OutNode)
{
	if (CurrentNPCId.IsEmpty()) { UE_LOG(LogTemp, Warning, TEXT("GetCurrentNode(...): NPCId is empty")); return false; }

	const FDialogueData* DialogueData = AllDialogues.Find(CurrentNPCId);
	if (!DialogueData) { UE_LOG(LogTemp, Warning, TEXT("GetCurrentNode(...): Did not find dialogue attached to NPCId: %s"), *CurrentNPCId); return false; }

	const FBurnTheVillageDialogueNode* Node = DialogueData->NodeMap.Find(CurrentNodeId);
	if (!Node) { UE_LOG(LogTemp, Warning, TEXT("GetCurrentNode(...): Did not find node attached to NodeId: %s , for NPCId: %s"), *CurrentNodeId, *CurrentNPCId); return false; }

	OutNode = *Node;
	return true;
}

TArray<FBurnTheVillageDialogueEdge> UBurnTheVillageDialogueManager::GetCurrentPlayerDialogueOptions()
{
	TArray<FBurnTheVillageDialogueEdge> Options;

	FBurnTheVillageDialogueNode CurrentNode;
	if (!GetCurrentNode(CurrentNode)) { UE_LOG(LogTemp, Warning, TEXT("GetCurrentPlayerDialogueOptions(): GetCurrentNode(...) failed, returning default")); return Options; }	//	This return might be an opportunity to set up a default dialogue return if process fails.

	const FDialogueData* DialogueData = AllDialogues.Find(CurrentNPCId);

	for (const FString& EdgeId : CurrentNode.OutgoingEdgeIds)
	{
		if (const FBurnTheVillageDialogueEdge* FoundEdge = DialogueData->EdgeMap.Find(EdgeId)) { Options.Add(*FoundEdge); }
		else { UE_LOG(LogTemp, Warning, TEXT("GetCurrentPlayerDialogueOptions(): Node '%s' references non-existent EdgeId '%s'"), *CurrentNode.NodeId, *EdgeId); }
	}
	return Options;	//	The non default return.
}

bool UBurnTheVillageDialogueManager::AdvanceDialogue(const FString& EdgeId)
{
	const FDialogueData* DialogueData = AllDialogues.Find(CurrentNPCId);
	if (!DialogueData) { UE_LOG(LogTemp, Warning, TEXT("ChooseOption(...): Failed to retrieve NPC dialogue")); return false; }

	const FBurnTheVillageDialogueEdge* ChosenEdge = DialogueData->EdgeMap.Find(EdgeId);
	if (!ChosenEdge) { UE_LOG(LogTemp, Warning, TEXT("ChooseOption(...): Failed to retrieve ChosenEdge with EdgeId: %s"), *EdgeId); return false; }

	if (!DialogueData->NodeMap.Contains(ChosenEdge->NextNodeId)) 
	{ 
		UE_LOG(LogTemp, Log, TEXT("ChooseOption(...): No Node found to come after ChosenEdge, ending dialogue"));
		EndDialogue();
		return false;	//	Dialogue not advanced, therefore returns false.
	}

	CurrentNodeId = ChosenEdge->NextNodeId;
	return true;
}

void UBurnTheVillageDialogueManager::EndDialogue()
{
	CurrentNPCId.Empty();
	CurrentNodeId.Empty();
}

FString UBurnTheVillageDialogueManager::GetNPCDialogueContent(const FBurnTheVillageDialogueNode& Node) const
{
	return Node.NPCDialogue;
}

FString UBurnTheVillageDialogueManager::GetPlayerDialogueContent(const FBurnTheVillageDialogueEdge& PlayerEdge) const
{
	return PlayerEdge.PlayerDialogue;
}
