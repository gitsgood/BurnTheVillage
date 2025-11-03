// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BurnTheVillageInteractInterface.h"	//	Interaction Interface implemented in C++ BECAUSE WE'RE FUCKING CHADS, WE'RE IN IT FOR THE LOVE OF THE GAME, WE BREEATH C++, WE LIVE THIS SHIT
#include "BurnTheVillageNPC.generated.h"

class UWidgetComponent;
class UBurnTheVillageDialogueWidget;

UCLASS()
class BURNTHEVILLAGE_API ABurnTheVillageNPC : public ACharacter, public IBurnTheVillageInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABurnTheVillageNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//	THIS CODE BELONGS TO US -------------------------------------------------------------------------------------------------------

protected:
	//	The NPCId variable from the dialogue jsons ought to also be used like their names, they will be the one and the same.
	//	At this moment, design wise, it's up to us to manually name them to make sure everything works.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	FString NPCId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UWidgetComponent* Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UCapsuleComponent* CapsuleTrigger;

	UPROPERTY()
	UBurnTheVillageDialogueWidget* OngoingDialogueWidgetInstance;

public:
	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	FString GetNPCId() const;

	UFUNCTION()
	virtual void ShowInteract(AActor* Interactor, bool bIsInRange) override;

	UFUNCTION()
	virtual void InitiateInteraction(AActor* Interactor) override;

	UFUNCTION()
	virtual void HideInteract(AActor* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Dialogue")
	TSubclassOf<UBurnTheVillageDialogueWidget> DialogueWidgetClass;
};