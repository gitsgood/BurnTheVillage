// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "PathfinderModule.h"

PathfinderModule::PathfinderModule() // constructor implementation
{
}

//  THE DIVINE NAVIGATION MESH SAMPLER. Blessed be the forgers of iron and the spikes and the barbwire
TArray<FVector> PathfinderModule::SampleNodesOverWholeNavMesh(float GridSpacing)
{
    //  Spawn Array of Vectors
    TArray<FVector> Points;

    //  call NavSys and from within in fetch world, if fails, end process
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return Points;

    //  recast the mesh from the whole set of navigation data we just pulled, if it containts no navmesh, end process
    ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavSys->GetDefaultNavDataInstance());
    if (!NavMesh) return Points;

    //  set up some control variables
    FBox NavMeshBounds = NavMesh->GetNavMeshBounds();
    FVector Min = NavMeshBounds.Min;
    FVector Max = NavMeshBounds.Max;

    //  Glorious nested FOR loops for going throiugh the grid like Gendarmerie, GridSpacing is determined by naked variable editable in Editor, declared in header
    for (float X = Min.X; X <= Max.X; X += GridSpacing)
    {
        for (float Y = Min.Y; Y <= Max.Y; Y += GridSpacing) //  Yes, Grid can indeed only be square, sue me
        {
            FVector TestPoint(X, Y, (Min.Z + Max.Z) / 2);
            FNavLocation Projected;
            if (NavSys->ProjectPointToNavigation(TestPoint, Projected)) //  Crux of the whole operation, hammered together by hedge magic and high wizardry of Tim Sweeney's band of North Carolina's council of sorcerers (I have no idea how this works, it's 1500+ lines of code that makes this shit operate and I suspect that is still just a wrapper)
            {
                Points.Add(Projected.Location);
                DrawDebugPoint(GetWorld(), Projected.Location, 5.f, FColor::Green, true, 10.f);
            }
        }
    }

    //  If nothing above fails, return should be a array full of navigable points in sampled spacing.
    return Points;
}

//  THE HALLOWED GRAPH BUILDER. Blessed be the stone cutters - It took a quarry to bury the dreams
TArray<FAbstractNodeForNavigation> PathfinderModule::BuildGraph(const TArray<FVector>& Points, float ConnectionRadius)
{
    // Instantiate Graph struct we defined in the header
    TArray<FAbstractNodeForNavigation> Graph;
    //  Make sure that it is only as big as there are Points in the array
    Graph.SetNum(Points.Num());

    //  ΟΡΚΙΖΟΜΑΙ ΣΤΗΝ ΙΔΕΑ ΑΥΤΗ
    for (int32 i = 0; i < Points.Num(); ++i)
    {
        Graph[i].HereWeAre = Points[i]; //  starts sampling points by indexes

        for (int32 j = 0; j < Points.Num(); ++j)
        {
            if (i == j) continue;   // overlapping points get discarded

            if (FVector::Dist(Points[i], Points[j]) <= ConnectionRadius)    //  Valid nodes are connected
            {
                Graph[i].HereWeCanGo.Add(j);
                DrawDebugLine(GetWorld(), Points[i], Points[j], FColor::Blue, true, 30.f);
            }
        }
    }

    //  If nothing above fails, retun should be a Graph consisting of Abstract nodes for navigation.
    return Graph;
}
