// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "DrawDebugHelpers.h"

struct FAbstractNodeForNavigation
{
	FVector HereWeAre;
	TArray<int32> HereWeCanGo;
	FAbstractNodeForNavigation() = default;
	// I have implicit constructor because Matt hates constructors for no good reason
};

struct GremlinsAStarNode
{
	int32 CurrentNodeIndex;
	int32 ParentNodeIndex;
	float RealCost;														//	also known as g cost
	float HeuristicsCost;												//	also known as h cost
	float EstimatedCost();	//	also known as f cost
	GremlinsAStarNode() = default;
	GremlinsAStarNode(int32 CNI, int PNI, float RC, float HC) : CurrentNodeIndex(CNI), ParentNodeIndex(PNI), RealCost(RC), HeuristicsCost(HC) {};
};

class PathfinderModule
{
public:
	PathfinderModule() = default; //	constructor

public: //	list of members, public because fuck access rights

	//	How far apart individual nodes are
	float NodeSpacingSampleRate = 65.f;

	//	How dense the connection is suppoosed to be, higher = denser, 0 = unconnected
	float ConnectionDensity = 1.6f;

public: //	list of methods, public because fuck accesss rights

	//	method to sample whole mesh, accepts NodeSpacingSampleRate as parameter
	TArray<FVector> SampleNodesOverWholeNavMesh(UWorld* World, float GridSpacing);

	//	method to build graph based on sampled nodes from function above, pulls them from array of Points and connects them based on ConnectionRadius parameter, constructed in runtime by multiplyig NodeSpacingSampleRate by ConnectionDensity
	TArray<FAbstractNodeForNavigation> BuildGraph(const TArray<FVector>& Points, float ConnectionRadius);
	//	overload of above for debugging purposes
	TArray<FAbstractNodeForNavigation> BuildGraph(const TArray<FVector>& Points, float ConnectionRadius, UWorld* World);


	//	method to find the closest node to player's position to bootstrap movement
	int32 FindClosestNode(const FVector& Pos, const TArray<FAbstractNodeForNavigation>& Graph);

	//	method to actually pathfind, you know, the actual reason why all this claptrap exists instead of just being relegated to UNavigationSystemV1 that already exists and does all this better
	TArray<int32> GremlinsHikingAdventure(const TArray<FAbstractNodeForNavigation>& Graph, int32 StartIndex, int32 GoalIndex);


	// +++BELOW THIS LINE IS LEGACY TRASH THAT EXISTS SOLELY TO POINT OUT HOW PSYCHOTIC MY THOUGHT PROCESS WAS!+++    **********************************************************************************************
//protected:

	//TArray<FVector> SampleNavMeshPoints(float Radius = 2000.f, float GridSpacing = 200.f);


};
