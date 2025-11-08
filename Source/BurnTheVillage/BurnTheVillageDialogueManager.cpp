// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnTheVillageDialogueManager.h"
#include "Misc/FileHelper.h"
#include "JsonObjectConverter.h"
#include "BurnTheVillageDialogue.h"
#include "BTVLoggingControlMacro.h"				//	This header contains ONLY a conditional macro enabling or disabling logging for convenience and eventually performance.

bool UBurnTheVillageDialogueManager::LoadDialogueFromFile(const FString& FilePath)
{
	//	We read the file, and take its contents as a string into JsonString.
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Filepath not found: %s"), TEXT(__FUNCTION__), *FilePath); return false; }

	//	Here, we do the deserialisation: we convert the JsonString into a FBurnTheVillageDialogueGraph type.
	FBurnTheVillageDialogueGraph LoadedDialogueGraph;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &LoadedDialogueGraph, 0, 0)) { BTV_LOG(LogTemp, Warning, TEXT("%s says: JSON deserialisation failed"), TEXT(__FUNCTION__)); return false; }

	//	If the dialogue data has no NPC identification, we do not let it cross this border.
	const FString& NPCIdFromFile = LoadedDialogueGraph.NPCId;
	if (NPCIdFromFile.IsEmpty()) { BTV_LOG(LogTemp, Warning, TEXT("%s says: NPCId is empty, aborting"), TEXT(__FUNCTION__)); return false; }

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
	BTV_LOG(LogTemp, Log, TEXT("%s says: Succesfully loaded dialogue for NPC '%s' with %d nodes and %d edges."), TEXT(__FUNCTION__), *NPCIdFromFile, NewDialogueData.NodeMap.Num(), NewDialogueData.EdgeMap.Num());
	return true;
}

bool UBurnTheVillageDialogueManager::InitiateConversationState(const FString& NPCId, const FString& StartNodeId)
{
	const FDialogueData* DialogueData = AllDialogues.Find(NPCId);
	if (!DialogueData) { BTV_LOG(LogTemp, Warning, TEXT("%s says: No dialogue found for this NPCId: %s"), TEXT(__FUNCTION__), *NPCId); return false; }

	if (!DialogueData->NodeMap.Contains(StartNodeId)) { BTV_LOG(LogTemp, Warning, TEXT("%s says: StartingNode %s not found for NPCId %s"), TEXT(__FUNCTION__), *StartNodeId, *NPCId); return false; }

	CurrentNPCId = NPCId;
	CurrentNodeId = StartNodeId;
	return true;
}

bool UBurnTheVillageDialogueManager::GetCurrentNode(FBurnTheVillageDialogueNode& OutNode)
{
	if (CurrentNPCId.IsEmpty()) { BTV_LOG(LogTemp, Warning, TEXT("%s says: NPCId is empty"), TEXT(__FUNCTION__)); return false; }

	const FDialogueData* DialogueData = AllDialogues.Find(CurrentNPCId);
	if (!DialogueData) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Did not find dialogue attached to NPCId: %s"), TEXT(__FUNCTION__), *CurrentNPCId); return false; }

	const FBurnTheVillageDialogueNode* Node = DialogueData->NodeMap.Find(CurrentNodeId);
	if (!Node) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Did not find node attached to NodeId: %s , for NPCId: %s"), TEXT(__FUNCTION__), *CurrentNodeId, *CurrentNPCId); return false; }

	OutNode = *Node;
	return true;
}

TArray<FBurnTheVillageDialogueEdge> UBurnTheVillageDialogueManager::GetCurrentPlayerDialogueOptions()
{
	TArray<FBurnTheVillageDialogueEdge> Options;

	FBurnTheVillageDialogueNode CurrentNode;
	if (!GetCurrentNode(CurrentNode)) { BTV_LOG(LogTemp, Warning, TEXT("%s says: GetCurrentNode(...) failed, returning default"), TEXT(__FUNCTION__)); return Options; }	//	This return might be an opportunity to set up a default dialogue return if process fails.

	const FDialogueData* DialogueData = AllDialogues.Find(CurrentNPCId);

	for (const FString& EdgeId : CurrentNode.OutgoingEdgeIds)
	{
		if (const FBurnTheVillageDialogueEdge* FoundEdge = DialogueData->EdgeMap.Find(EdgeId)) { Options.Add(*FoundEdge); }
		else { BTV_LOG(LogTemp, Warning, TEXT("%s says: Node '%s' references non-existent EdgeId '%s'"), TEXT(__FUNCTION__), *CurrentNode.NodeId, *EdgeId); }
	}
	return Options;	//	The non default return.
}

bool UBurnTheVillageDialogueManager::AdvanceDialogue(const FString& EdgeId)
{
	const FDialogueData* DialogueData = AllDialogues.Find(CurrentNPCId);
	if (!DialogueData) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve NPC dialogue"), TEXT(__FUNCTION__)); return false; }

	const FBurnTheVillageDialogueEdge* ChosenEdge = DialogueData->EdgeMap.Find(EdgeId);
	if (!ChosenEdge) { BTV_LOG(LogTemp, Warning, TEXT("%s says: Failed to retrieve ChosenEdge with EdgeId: %s"), TEXT(__FUNCTION__), *EdgeId); return false; }

	if (!DialogueData->NodeMap.Contains(ChosenEdge->NextNodeId)) 
	{ 
		BTV_LOG(LogTemp, Log, TEXT("%s says: No Node found to come after ChosenEdge, ending dialogue"), TEXT(__FUNCTION__));
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
