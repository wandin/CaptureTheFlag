// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CTFSpawnArea.generated.h"

UCLASS()
class CAPTURETHEFLAG_API ACTFSpawnArea : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ACTFSpawnArea();

	// Box component to define the spawn area
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* SpawnAreaBox;
	
	// Function to get a random point within the spawn area
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	FVector GetRandomPointInArea() const;


	UPROPERTY(EditDefaultsOnly)
	int32 TeamID;
};
