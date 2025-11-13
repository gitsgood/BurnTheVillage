// Copyright Epic Games, Inc. All Rights Reserved.

#include "BurnTheVillagePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "BurnTheVillage.h"
#include "BurnTheVillageCharacter.h"		//	I include this so that when we GetLocalPlayer, we cast it into this to call the actual implementation of the interact function.
#include "BTVLoggingControlMacro.h"			//	This header contains ONLY a conditional macro enabling or disabling logging for convenience and eventually performance.

ABurnTheVillagePlayerController::ABurnTheVillagePlayerController()
{
	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ABurnTheVillagePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Only set up input on local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// Setup mouse input events
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ABurnTheVillagePlayerController::OnInputStarted);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ABurnTheVillagePlayerController::OnSetDestinationTriggered);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ABurnTheVillagePlayerController::OnSetDestinationReleased);
			EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ABurnTheVillagePlayerController::OnSetDestinationReleased);

			// Setup touch input events
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ABurnTheVillagePlayerController::OnInputStarted);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ABurnTheVillagePlayerController::OnTouchTriggered);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ABurnTheVillagePlayerController::OnTouchReleased);
			EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ABurnTheVillagePlayerController::OnTouchReleased);

			//	YEEEAAHAHAHA, WE SET UP THE INTERACTION BUTTON RIGHT HERE MOUHAHAHAHAHA
			EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ABurnTheVillagePlayerController::OnInteractionPressed);
		}
		else
		{
			UE_LOG(LogBurnTheVillage, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		}
	}
}

void ABurnTheVillagePlayerController::OnInputStarted()
{
	StopMovement();
}

void ABurnTheVillagePlayerController::OnSetDestinationTriggered()
{
	if (bUseCustomPathfinding) return;

	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

//	Original Epic's Implementation commented out below

//void ABurnTheVillagePlayerController::OnSetDestinationReleased()
//{
//	// If it was a short press
//	if (FollowTime <= ShortPressThreshold)
//	{
//		// We move there and spawn some particles
//		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
//		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
//	}
//
//	FollowTime = 0.f;
//}

//	My psychotic replacement implementation, may gods above and below have mercy on my wretched soul

void ABurnTheVillagePlayerController::OnSetDestinationReleased()
{
	FHitResult Hit;
	TArray<int32> TrailIndexes;
	if (bIsTouch)
	{
		GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	if (!Hit.bBlockingHit) return;

	// Click location
	FVector ClickLocation = Hit.Location;

	// Find nearest correct nodes
	int32 PlayerPosIndex = PathingGremlin.FindClosestNode(GetPawn()->GetActorLocation(), Graph);
	int32 ClickNodeIndex = PathingGremlin.FindClosestNode(ClickLocation, Graph);
	UE_LOG(LogTemp, Warning, TEXT("Mouse click closest to node index: %d"), ClickNodeIndex);
	if (ClickNodeIndex == INDEX_NONE) return;

	// Find path using A*
	if (PlayerPosIndex != INDEX_NONE || ClickNodeIndex != INDEX_NONE)
	{
		TrailIndexes = PathingGremlin.GremlinsHikingAdventure(Graph, PlayerPosIndex, ClickNodeIndex);
	}
	if (TrailIndexes.Num() == 0) return;

	//	Convert node indexes to world locations
	TArray<FVector> TrailPath;
	for (int32 Index : TrailIndexes)
	{
		TrailPath.Add(Graph[Index].HereWeAre);
	}

	//	Somehow call function that turns all those vectors into actual movement commands
	SetPath(TrailPath);
}

void ABurnTheVillagePlayerController::SetPath(const TArray<FVector>& ArrayOfLocations)
{
	PathPoints = ArrayOfLocations;
	CurrentPathIndex = 0;
}

void ABurnTheVillagePlayerController::MoveCharacterAlongPath(float deltaTime)
{
	APawn* ControlledPawn = GetPawn();
	if (PathPoints.Num() == 0 || CurrentPathIndex >= PathPoints.Num())
		return;

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	FVector Target = PathPoints[CurrentPathIndex];

	float Distance = FVector::Dist2D(CurrentLocation, Target);
	if (Distance < AcceptanceRadius)
	{
		CurrentPathIndex++;
		return;
	}
	FVector Direction = (Target - CurrentLocation);
	Direction.Z = 0; // optional, ignore vertical if desired
	Direction.Normalize();

	ControlledPawn->AddMovementInput(Direction, 1.0f);
}

// Triggered every frame when the input is held down
void ABurnTheVillagePlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ABurnTheVillagePlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ABurnTheVillagePlayerController::OnInteractionPressed()
{
	BTV_LOG(LogTemp, Log, TEXT("%s says: Interaction button press is detected!"), TEXT(__FUNCTION__));
	ABurnTheVillageCharacter* PlayerCharacter = Cast<ABurnTheVillageCharacter>(GetCharacter());
	if (!PlayerCharacter) return;
	PlayerCharacter->OnInteractionStarted();
	BTV_LOG(LogTemp, Log, TEXT("%s says: Succesfully called %s's OnInteractionStarted function!"), TEXT(__FUNCTION__), *PlayerCharacter->GetName());
}

void ABurnTheVillagePlayerController::BeginPlay()
{
	Super::BeginPlay();

	//  Initial sanity check log to ensure controller is working
	BTV_LOG(LogTemp, Warning, TEXT("Controller works."), *GetName());

	if (GetPawn())
	{
		BTV_LOG(LogTemp, Warning, TEXT("Controller molests pawn"), *GetPawn()->GetName());
	}

	if (!GetWorld()) return;

	//  Fetch World's navigation data in its entirety
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys) return;

	//  Find Navmesh among that trash
	ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavSys->GetDefaultNavDataInstance());
	if (!NavMesh)
	{
		// Call Matt retarded if case he forgot to include NavMesh
		BTV_LOG(LogTemp, Error, TEXT("No NavMesh found! RETARD!"));
		return;
	}

	//  ALMOST Call Matt a good boi in case he didn't
	BTV_LOG(LogTemp, Warning, TEXT("NavMesh found, good job, you didn't forget! %s"), *NavMesh->GetName());

	//  Fetch NavMesh data and store it in NavMesh directly from the world, making everything above this line pointless
	NavMesh = Cast<ARecastNavMesh>(FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->GetDefaultNavDataInstance());

	if (!NavMesh) return;

	//  Get size of the NavMesh
	FBox NagivationVolume = NavMesh->GetNavMeshBounds();

	//  Gerate array of Vectors which the Pathfinding Gremlin then populates with nodes based on samples
	NavPoints = PathingGremlin.SampleNodesOverWholeNavMesh(GetWorld(), PathingGremlin.NodeSpacingSampleRate);

	//  Make the Gremlin construct a fucking graph out of the samples
	float NodeConnectionRadius = PathingGremlin.NodeSpacingSampleRate * PathingGremlin.ConnectionDensity;
	Graph = PathingGremlin.BuildGraph(NavPoints, NodeConnectionRadius, GetWorld());

	//  attach a number to just how wasteful Matt is with processing cycles 
	BTV_LOG(LogTemp, Warning, TEXT("Graph built with %d nodes."), Graph.Num());
}

void ABurnTheVillagePlayerController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	MoveCharacterAlongPath(deltaTime);
}


