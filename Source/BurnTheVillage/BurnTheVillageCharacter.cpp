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
#include "BurnTheVillageDialogueManager.h"		//	Our own lil dialogue manager class, obviously
#include "Misc/FileHelper.h"					//	Need this for some file I/O magic
#include "Components/SphereComponent.h"			//	A TRIGGER SPHERE, IN CEE PEE PEE
#include "BurnTheVillageInteractInterface.h"	//	Our interaction interface, to let the functions here know what the fuck we're talking about when we ask if it's implemented elsewhere.
#include "BurnTheVillageNPC.h"					//	Interaction involves casting into an NPC, granting us ACCESS TO THEIR VALUABLE ID (and functions).

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

	//	I HAVE NO TRIGGER SPHERE AND I MUST TRIGGER
	SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	SphereTrigger->SetupAttachment(RootComponent);
	SphereTrigger->SetSphereRadius(32.f);
	SphereTrigger->SetLineThickness(0.f);
	SphereTrigger->SetWorldScale3D(FVector(5.f, 5.f, 5.f));
	SphereTrigger->SetGenerateOverlapEvents(true);
	SphereTrigger->SetCollisionProfileName(FName("Trigger"));
	SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABurnTheVillageCharacter::OnTriggerSphereBeginOverlap);

	//	WE WILL INSTANTIATE AN OBJECT OF TYPE DIALOGUE MANAGER IN THE CONSTRUCTOR
	DialogueManager = CreateDefaultSubobject<UBurnTheVillageDialogueManager>(TEXT("DialogueManager"));	//	As I understand it, we create a default object (within the character object, hence the "subObject").
}

void ABurnTheVillageCharacter::BeginPlay()
{
	Super::BeginPlay();

	//	For some fucking reason, the constructor failed to create the manager even though it used to work, so this line will try again if the other fails. THE DIALOGUE MANAGER MUST EXIST NO MATTER WHAT.
	if (!DialogueManager)
	{
		DialogueManager = NewObject<UBurnTheVillageDialogueManager>(this);
		ensure(DialogueManager);
	}

	// AT BEGIN PLAY, WE OUGHTA LOAD ALL THE DIALOGUES FROM FILES, I'm using a lamda for this as this only needs to be done once
	auto DialogueJsonsArray = []()
		{
			TArray<FString> FoundFilesArray;
			FString DialoguesFilePath{ FPaths::ProjectContentDir() + TEXT("OurFuckingFolder/DialogueJSONs/") };
			IFileManager::Get().FindFiles(FoundFilesArray, *DialoguesFilePath);
			return FoundFilesArray;
		};
	for (FString DialogueJsonPath : DialogueJsonsArray())
	{
		if (!DialogueManager)
		{
			UE_LOG(LogTemp, Error, TEXT("ABurnTheVillageCharacter::BeginPlay() says: constructor failed to instantiate a dialogue manager class."))
			break;
		}
		FString DialoguesFilePath{ FPaths::ProjectContentDir() + TEXT("OurFuckingFolder/DialogueJSONs/") + DialogueJsonPath};
		DialogueManager->LoadDialogueFromFile(DialoguesFilePath);
		UE_LOG(LogTemp, Warning, TEXT("ABurnTheVillageCharacter::BeginPlay() says: Added contents of %s into DialogueManager"), *DialoguesFilePath)
	}
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
		UE_LOG(LogTemp, Error, TEXT("ABurnTheVillageCharacter() says: DIALOGUE MANAGER HAS NOT BEEN SET"));
		return nullptr;
	}
	return DialogueManager;
}

void ABurnTheVillageCharacter::OnTriggerSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->Implements<UBurnTheVillageInteractInterface>()) return;
	bIsOverlappingInteractable = true;
	CurrentInteractableActor = OtherActor;
	UE_LOG(LogTemp, Log, TEXT("Trigger Sphere has detected an interface"))
	
	//	Some people say "cast to interface for blablabla". I say no to that. I want this cast for the most precious thing of all... THEIR FUCKING ID.
	ABurnTheVillageNPC* InteractedWithNPC = Cast<ABurnTheVillageNPC>(OtherActor);
	if (!InteractedWithNPC) return;

	InteractedWithNPC->ShowInteract(OtherActor, bFromSweep);
	this->SetCurrentNPCIDThatIsTalkedTo(InteractedWithNPC->GetNPCId());
}

void ABurnTheVillageCharacter::OnTriggerSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CurrentInteractableActor)
	{
		bIsOverlappingInteractable = false;
		CurrentInteractableActor = nullptr;
	}
}

void ABurnTheVillageCharacter::OnInteractionStarted()
{
	UE_LOG(LogTemp, Log, TEXT("Interaction action succesfully triggered"))
	if (!bIsOverlappingInteractable || !CurrentInteractableActor) return;
}
