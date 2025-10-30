// Copyright Epic Games, Inc. All Rights Reserved.

#include "BurnTheVillageCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "BurnTheVillageDialogueManager.h"

ABurnTheVillageCharacter::ABurnTheVillageCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// WE WILL INSTANTIATE AN OBJECT OF TYPE DIALOGUE MANAGER IN THE CONSTRUCTOR
	DialogueManager = CreateDefaultSubobject<UBurnTheVillageDialogueManager>(TEXT("DialogueManager"));	//	As I understand it, we create a default object (within the character object, hence the "subObject").
}

void ABurnTheVillageCharacter::BeginPlay()
{
	Super::BeginPlay();

	// stub
}

void ABurnTheVillageCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// stub
}

UBurnTheVillageDialogueManager* ABurnTheVillageCharacter::GetDialogueManager() const
{
	if (!DialogueManager)
	{
		UE_LOG(LogTemp, Error, TEXT("DIALOGUE MANAGER HAS NOT BEEN GOT"));
		return nullptr;
	}
	return DialogueManager;
}
