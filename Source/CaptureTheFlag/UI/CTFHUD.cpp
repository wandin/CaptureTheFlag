// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFHUD.h"

#include "CTFGameplayWidget.h"
#include "CTFTeamSelectionWidget.h"
#include "CaptureTheFlag/PlayerController/CTFPlayerController.h"
#include "Blueprint/UserWidget.h"


void ACTFHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ACTFHUD::AddTeamSelectionWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && TeamSelectionClass)
	{
		TeamSelectionWidget = CreateWidget<UCTFTeamSelectionWidget>(PlayerController, TeamSelectionClass);
		TeamSelectionWidget->AddToViewport();
	}
}

void ACTFHUD::RemoveTeamSelectionWidgetFromScreen()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && TeamSelectionClass)
	{
		TeamSelectionWidget->RemoveFromParent();
	}
}

void ACTFHUD::AddGameplayWidget()
{
	ACTFPlayerController* CTFPlayerController = Cast<ACTFPlayerController>(GetOwningPlayerController());
	if (CTFPlayerController && GameplayWidgetClass)
	{
		ACTFCharacter* Character = CTFPlayerController->GetCharacterRef();
		if(Character)
		{
			GameplayWidget = CreateWidget<UCTFGameplayWidget>(CTFPlayerController, GameplayWidgetClass);
			if (GameplayWidget)
			{
				GameplayWidget->AddToViewport();
			}
		}
	}
}
