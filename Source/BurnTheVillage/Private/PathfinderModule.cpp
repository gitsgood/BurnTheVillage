// Fill out your copyright notice in the Description page of Project Settings.


#include "PathfinderModule.h"

//  THE DIVINE NAVIGATION MESH SAMPLER. Blessed be the forgers of iron and the spikes and the barbwire
TArray<FVector> PathfinderModule::SampleNodesOverWholeNavMesh(UWorld* World, float GridSpacing)
{
    TArray<FVector> Points;

    //  No world, no logic
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("SampleNodesOverWholeNavMesh: Invalid World pointer!"));
        return Points;
    }

    //  Spawn Array of Vectors

    //  call NavSys and from within in fetch world, if fails, end process
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
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
                DrawDebugPoint(World, Projected.Location, 5.f, FColor::Green, true, 10.f);
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
            }
        }
    }

    //  If nothing above fails, retun should be a Graph consisting of Abstract nodes for navigation.
    return Graph;
}

TArray<FAbstractNodeForNavigation> PathfinderModule::BuildGraph(const TArray<FVector>& Points, float ConnectionRadius, UWorld* World)
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
                DrawDebugLine(World, Points[i], Points[j], FColor::Blue, true, 30.f);
            }
        }
    }

    //  If nothing above fails, retun should be a Graph consisting of Abstract nodes for navigation.
    return Graph;
}


//int32 PathfinderModule::FindClosestNode(const FVector& Pos, const TArray<FAbstractNodeForNavigation>& Graph)
//{
//    float BestDist = TNumericLimits<float>::Max();
//    int32 BestIndex = -1;
//    for (int32 i = 0; i < Graph.Num(); ++i)
//    {
//        float D = FVector::Dist(Pos, Graph[i].HereWeAre);
//        if (D < BestDist)
//        {
//            BestDist = D;
//            BestIndex = i;
//        }
//    }
//    return BestIndex;
//}

//TArray<int32> PathfinderModule::GremlinsHikingAdventure(const TArray<FAbstractNodeForNavigation>& Graph, int32 StartPosIndex, int32 GoalPosIndex)
//{
//    //  the most basic of error checks
//    if (StartIndex < 0 || GoalIndex < 0 || StartPosIndex >= Graph.Num() || GoalPosIndex >= Graph.Num())
//        return {};
//
//    //  As the name suggest, array of nodes we need to check
//    TArray<GremlinsAStarNode> NodesTODO;
//
//    //  As the name suggest, array of nodes we already checked
//    TArray<GremlinsAStarNode> NodesDONE;
//
//    //  Add start to the list, it has no parent so -1, real cost is 0 as we didn't move yet, heuristics cost is according to manhattan distance (I hope, math is not my forte)
//    NodesTODO.Add(GremlinsAStarNode(StartPosIndex, -1, 0.f, FVector::Dist(Graph[StartPosIndex].HereWeAre, Graph[GoalPosIndex].HereWeAre)));
//
//    //  While we still have nodes in the list, keep searching
//    while (NodesTODO.Num() > 0) //  Realistically, if this ever gets cut because TODO list becomes empty it means there is no path
//    {
//        int32 Current = 0;
//        for (int32 i = 1; i < NodesTODO.Num(); ++i) //  Run EstimatedCost function which adds up real and heuristics cost for Estimated cost, the whole g, h and f values
//            if (NodesTODO[i].EstimatedCost() < NodesTODO[Current].EstimatedCost())
//                Current = i;
//
//        //  Pulls the node out of the TODO array and adds it to DONE array
//        GremlinsAStarNode CurrentNode = NodesTODO[Current];
//        NodesTODO.RemoveAt(Current);
//        NodesDONE.Add(CurrentNode);
//
//        if (CurrentNode.CurrentNodeIndex == GoalPosIndex)   // If we overlap with the goal we're done
//        {
//            // SACRED PATH BUILDER. Blessed be the woodworkers, the crosses and the gallows
//            TArray<int32> GoblinTrail;
//
//            //  However the above gremlin got to the exit, we now retrace our steps to reach back the original position by going by the connections of current to their parent
//            GoblinTrail.Add(CurrentNode.CurrentNodeIndex);
//            int32 Parent = CurrentNode.ParentNodeIndex;
//            while (Parent != -1)
//            {
//                GoblinTrail.Insert(Parent, 0); //We keep inserting them to the beginning so the path actually builds from start to finish
//                for (const GremlinsAStarNode& Node : NodesDONE)
//                {
//                    if (Node.CurrentNodeIndex == Parent)
//                    {
//                        Parent = Node.ParentNodeIndex;
//                        break;
//                    }
//                }
//            }
//            //  Log the number of nodes we got to travel
//            UE_LOG(LogTemp, Warning, TEXT("A* path found with %d nodes."), GoblinTrail.Num());
//
//            // If nothing above fails, the return should be array of nodes containg the path
//            return GoblinTrail;
//        }
//
//        // ΠΟΝΟΣ ΜΑΧΗ ΟΡΑΜΑ
//        for (int32 Neighbor : Graph[CurrentNode.CurrentNodeIndex].HereWeCanGo)
//        {
//            //  check if the neighbour has been visited already or not
//            bool VisitedOrNot = false;
//            for (const GremlinsAStarNode& CurrentDoneNode : NodesDONE)
//                if (CurrentDoneNode.CurrentNodeIndex == Neighbor)
//                    VisitedOrNot = true;
//            if (VisitedOrNot) continue;
//
//            //  calculate real cost from start through this node to its neighbour
//            float RealCost = CurrentNode.RealCost + FVector::Dist(Graph[CurrentNode.CurrentNodeIndex].HereWeAre, Graph[Neighbor].HereWeAre);
//
//            // calculate heuristics
//            float HeuristicsCost = FVector::Dist(Graph[Neighbor].HereWeAre, Graph[GoalPosIndex].HereWeAre);
//
//            //  Check if this neighbour is already on the TODO list
//            GremlinsAStarNode* Existing = NodesTODO.FindByPredicate([Neighbor](const GremlinsAStarNode& N) { return N.CurrentNodeIndex == Neighbor; }); //  FindByPredicate generously funded and supported by Epic Games LTD
//
//            if (!Existing) { //   if not, add as a candidate
//                GremlinsAStarNode NewNode;
//                NewNode.CurrentNodeIndex = Neighbor;
//                NewNode.ParentNodeIndex = CurrentNode.CurrentNodeIndex;
//                NewNode.RealCost = RealCost;
//                NewNode.HeuristicsCost = HeuristicsCost;
//
//                NodesTODO.Add(NewNode);
//            }
//
//            else if (RealCost < Existing->RealCost) //  if already on TODO list but we found cheaper path, update cost and parent
//            {
//                Existing->RealCost = RealCost;
//                Existing->ParentNodeIndex = CurrentNode.CurrentNodeIndex;
//            }
//        }
//    }
//
//    UE_LOG(LogTemp, Warning, TEXT("No path found!"));
//    //  Something went tits up, return nothing
//    return {};
//}

// BELOW THIS LINE IS LEGACY TRASH THAT EXISTS SOLELY TO POINT OUT HOW PSYCHOTIC MY THOUGHT PROCESS WAS!    ----------------------------------------------------------------------------------------

//this is from way back when this used to live in the AI controller and get generated around a pawn during AI Controller's BeginPlay function


//  +++ ANYTHING BELOW THIS LINE IS A LEGACY CODE FOR POSTERITY, NOT PRODUCTION CODE

//TArray<FVector> AMyAIController::SampleNavMeshPoints(float Radius, float GridSpacing)
//{
//    TArray<FVector> Points;
//
//    if (!GetWorld() || !GetPawn()) return Points;
//
//    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
//    if (!NavSys) return Points;
//
//    FVector Center = GetPawn()->GetActorLocation();
//
//    for (float X = Center.X - Radius; X <= Center.X + Radius; X += GridSpacing)
//    {
//        for (float Y = Center.Y - Radius; Y <= Center.Y + Radius; Y += GridSpacing)
//        {
//            FVector TestPoint(X, Y, Center.Z);
//            FNavLocation Projected;
//            if (NavSys->ProjectPointToNavigation(TestPoint, Projected))
//            {
//                Points.Add(Projected.Location);
//                DrawDebugSphere(GetWorld(), Projected.Location, 20.f, 6, FColor::Green, true, 30.f);
//            }
//        }
//    }
//
//    UE_LOG(LogTemp, Warning, TEXT("Sampled %d navmesh points."), Points.Num());
//    return Points;
//}

//float GremlinsAStarNode::EstimatedCost()
//{
//    return GremlinsAStarNode::RealCost + GremlinsAStarNode::HeuristicsCost;;
//}
