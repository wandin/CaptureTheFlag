// CTFFlag.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTFFlag.generated.h"

class UBoxComponent;

UCLASS()
class CAPTURETHEFLAG_API ACTFFlag : public AActor
{
	GENERATED_BODY()
	
public:	
	ACTFFlag();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Skeletal mesh for the flag
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* FlagMesh;

	// Box component for collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;

	// Function to handle overlap with character
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Function to handle when the flag is dropped
	void DropFlag();

	// Reference to the character holding the flag
	UPROPERTY(ReplicatedUsing = OnRep_HeldBy, VisibleAnywhere, BlueprintReadOnly, Category = "Flag")
	class ACTFCharacter* HeldBy;

	// Function to handle replication of HeldBy
	UFUNCTION()
	void OnRep_HeldBy();

	// Function to handle flag capture
	void HandleCapture(ACTFCharacter* CapturingCharacter);

	// Function to handle flag being dropped
	void HandleDrop();
};

