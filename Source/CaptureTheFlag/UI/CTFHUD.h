// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CTFHUD.generated.h"

class UCTFGameplayWidget;
class UCTFTeamSelectionWidget;


/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API ACTFHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	/* Team Selection Widget */
	UPROPERTY(EditAnywhere, Category = "Selection Widget")
	TSubclassOf<UUserWidget> TeamSelectionClass;
	UPROPERTY()
	UCTFTeamSelectionWidget* TeamSelectionWidget;
	void AddTeamSelectionWidget();
	/* End of Team Selection */
	void RemoveTeamSelectionWidgetFromScreen();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> GameplayWidgetClass;
	UPROPERTY()
	UCTFGameplayWidget* GameplayWidget;


	void AddGameplayWidget();
};