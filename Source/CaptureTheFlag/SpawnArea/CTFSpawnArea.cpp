// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFSpawnArea.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

ACTFSpawnArea::ACTFSpawnArea()
{
	// Create box component and set it as the root component
	SpawnAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnAreaBox"));
	RootComponent = SpawnAreaBox;
}

FVector ACTFSpawnArea::GetRandomPointInArea() const
{
	if (!SpawnAreaBox) return FVector::ZeroVector;
	// Get a random point within the spawn area
	FVector Origin = SpawnAreaBox->GetComponentLocation();
	FVector BoxExtent = SpawnAreaBox->GetScaledBoxExtent();

	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);
	return RandomPoint;
}
