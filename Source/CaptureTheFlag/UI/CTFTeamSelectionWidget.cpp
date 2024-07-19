// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFTeamSelectionWidget.h"

#include "Components/Button.h"
#include "CaptureTheFlag/PlayerController/CTFPlayerController.h"

void UCTFTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ButtonTeam1)
	{
		ButtonTeam1->OnClicked.AddDynamic(this, &UCTFTeamSelectionWidget::OnTeam1Selected);
	}
	if(ButtonTeam2)
	{
		ButtonTeam2->OnClicked.AddDynamic(this, &UCTFTeamSelectionWidget::OnTeam2Selected);
	}
}

void UCTFTeamSelectionWidget::OnTeam1Selected()
{
	if(ACTFPlayerController* WDNPlayerController = Cast<ACTFPlayerController>(GetOwningPlayer()))
	{
		WDNPlayerController->SelectTeam(1);
	}
}

void UCTFTeamSelectionWidget::OnTeam2Selected()
{
	if(ACTFPlayerController* WDNPlayerController = Cast<ACTFPlayerController>(GetOwningPlayer()))
	{
		WDNPlayerController->SelectTeam(2);
	}
}
