// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"

struct FAbstractNodeForNavigation
{
	FVector HereWeAre;
	TArray<int32> HereWeCanGo;
	// I have implicit constructor because Matt hates constructors for no good reason
};

class BURNTHEVILLAGE_API PathfinderModule : public UObject
{
public:
	PathfinderModule(); //	constructor

public: //	list of members, public because fuck access rights

	//	How far apart individual nodes are
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation Sampling")
	float NodeSpacingSampleRate = 150.f;

	//	How dense the connection is suppoosed to be, higher = denser, 0 = unconnected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation Sampling")
	float ConnectionDensity = 3.0f;

public: //	list of methods, public because fuck accesss rights

	//	method to sample whole mesh, accepts NodeSpacingSampleRate as parameter
	TArray<FVector> SampleNodesOverWholeNavMesh(float GridSpacing);

	//	method to build graph based on sampled nodes from function above, pulls them from array of Points and connects them based on ConnectionRadius parameter, constructed in runtime by multiplyig NodeSpacingSampleRate by ConnectionDensity
	TArray<FAbstractNodeForNavigation> BuildGraph(const TArray<FVector>& Points, float ConnectionRadius);

};
