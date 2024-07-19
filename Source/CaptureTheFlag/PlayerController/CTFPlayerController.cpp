// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFPlayerController.h"

#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "CaptureTheFlag/GameModes/CTFGameMode.h"
#include "CaptureTheFlag/UI/CTFGameplayWidget.h"
#include "CaptureTheFlag/UI/CTFHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

class UEnhancedInputLocalPlayerSubsystem;

void ACTFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameHUD = Cast<ACTFHUD>(GetHUD());
	if(GameHUD)
	{
		GameHUD->AddTeamSelectionWidget();
		FInputModeUIOnly InputUI;
		SetInputMode(InputUI);
		SetShowMouseCursor(true);
	}
}

ACTFCharacter* ACTFPlayerController::GetCharacterRef()
{
	if (!CTFCharacter)
	{
		CTFCharacter = Cast<ACTFCharacter>(GetPawn());
		return CTFCharacter;
	}
	return CTFCharacter;
}

void ACTFPlayerController::SelectTeam(int32 TeamID)
{
	SelectedTeam = TeamID;
	
	Server_SelectTeam(SelectedTeam);
	if(GameHUD)
	{
		GameHUD->RemoveTeamSelectionWidgetFromScreen();
		FInputModeGameOnly GameMode;
		SetInputMode(GameMode);
		SetShowMouseCursor(false);
	}
}

void ACTFPlayerController::Server_SelectTeam_Implementation(int32 TeamID)
{
	if(ACTFGameModeBase* GM = Cast<ACTFGameModeBase>(GetWorld()->GetAuthGameMode()))
	{ 
		GM->HandleTeamSelection(this, TeamID);
	}
	if(GameHUD)
	{
		GameHUD->RemoveTeamSelectionWidgetFromScreen();
	}
}

void ACTFPlayerController::CreateGameplayWidget() const
{
	if(GameHUD)
	{
		GameHUD->AddGameplayWidget();
	}
}

void ACTFPlayerController::UpdatePlayerHealth(float Health, float MaxHealth)
{
	GameHUD = GameHUD == nullptr ? Cast<ACTFHUD>(GetHUD()) : GameHUD;

	bool bHUDValid = GameHUD && GameHUD->GameplayWidget && GameHUD->GameplayWidget->PlayerHealth;
	if(bHUDValid)
	{
		const float HealthPercentage = Health / MaxHealth;
 		GameHUD->GameplayWidget->PlayerHealth->SetPercent(HealthPercentage);
	}
	else
	{
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ACTFPlayerController::UpdatePlayerScore(float Score)
{
	GameHUD = GameHUD == nullptr ? Cast<ACTFHUD>(GetHUD()) : GameHUD;
	bool bHUDValid = GameHUD && GameHUD->GameplayWidget && GameHUD->GameplayWidget->KillsText;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		GameHUD->GameplayWidget->KillsText->SetText(FText::FromString(ScoreText));
	}
	else
	{
		HUDScore = Score;
	}
}

void ACTFPlayerController::UpdatePlayerDefeats(int32 Defeats)
{
	GameHUD = GameHUD == nullptr ? Cast<ACTFHUD>(GetHUD()) : GameHUD;
	bool bHUDValid = GameHUD &&	GameHUD->GameplayWidget && GameHUD->GameplayWidget->DeathsText;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		GameHUD->GameplayWidget->DeathsText->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		HUDDefeats = Defeats;
	}
}

void ACTFPlayerController::UpdateFlagScore(int32 FlagPoint)
{
	GameHUD = GameHUD == nullptr ? Cast<ACTFHUD>(GetHUD()) : GameHUD;
	bool bHUDValid = GameHUD &&	GameHUD->GameplayWidget && GameHUD->GameplayWidget->FlagCapturedText;
	if (bHUDValid)
	{
		FString FlagsText = FString::Printf(TEXT("%d"), FlagPoint);
		GameHUD->GameplayWidget->FlagCapturedText->SetText(FText::FromString(FlagsText));
	}
	else
	{
		EnemyFlagScore = FlagPoint;
	}
}

void ACTFPlayerController::UpdateEnemyFlagScore(int32 EnemyFlagPoint)
{
	GameHUD = GameHUD == nullptr ? Cast<ACTFHUD>(GetHUD()) : GameHUD;
	bool bHUDValid = GameHUD &&	GameHUD->GameplayWidget && GameHUD->GameplayWidget->EnemyFlagCapturedText;
	if (bHUDValid)
	{
		FString EnemyFlagsText = FString::Printf(TEXT("%d"), EnemyFlagPoint);
		GameHUD->GameplayWidget->EnemyFlagCapturedText->SetText(FText::FromString(EnemyFlagsText));
	}
	else
	{
		EnemyFlagScore = EnemyFlagPoint;
	}
}