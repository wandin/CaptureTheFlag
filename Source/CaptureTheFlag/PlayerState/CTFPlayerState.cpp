// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFPlayerState.h"

#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "CaptureTheFlag/PlayerController/CTFPlayerController.h"
#include "Net/UnrealNetwork.h"

ACTFPlayerState::ACTFPlayerState()
{
	TeamID = 0;
}

int32 ACTFPlayerState::GetTeamID() const
{
	return TeamID;
}

void ACTFPlayerState::SetTeamID(int32 NewTeamID)
{
	TeamID = NewTeamID;
}

void ACTFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTFPlayerState, TeamID);
	DOREPLIFETIME(ACTFPlayerState, Defeats);
	DOREPLIFETIME(ACTFPlayerState, FlagScore);
	DOREPLIFETIME(ACTFPlayerState, EnemyFlagScore);
}

void ACTFPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount); // Score variable is outdated on PlayerState, GetScore instead.
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if(CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if(CTFController)
		{
			CTFController->UpdatePlayerScore(GetScore());
		}
	}
	OnRep_Score();
}

void ACTFPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdatePlayerScore(GetScore());
		}
	}
}

void ACTFPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdatePlayerDefeats(Defeats);
		}
	}
	OnRep_Defeats();
}

void ACTFPlayerState::OnRep_Defeats()
{
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdatePlayerDefeats(Defeats);
		}
	}
}



void ACTFPlayerState::AddToFlagScore(int32 FlagScoreAmount)
{
	FlagScore += FlagScoreAmount;
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdateFlagScore(FlagScore);
		}
	}
	OnRep_FlagScore(); // Atualizar HUD imediatamente
}

void ACTFPlayerState::AddToEnemyFlagScore(int32 EnemyFlagScoreAmount)
{
	EnemyFlagScore += EnemyFlagScoreAmount;
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdateEnemyFlagScore(EnemyFlagScore);
		}
	}
	OnRep_EnemyFlagScore(); // Atualizar HUD imediatamente
}

void ACTFPlayerState::OnRep_FlagScore()
{
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdateFlagScore(FlagScore);
		}
	}
}

void ACTFPlayerState::OnRep_EnemyFlagScore()
{
	CTFCharacter = CTFCharacter == nullptr ? Cast<ACTFCharacter>(GetPawn()) : CTFCharacter;
	if (CTFCharacter)
	{
		CTFController = CTFController == nullptr ? Cast<ACTFPlayerController>(CTFCharacter->Controller) : CTFController;
		if (CTFController)
		{
			CTFController->UpdateEnemyFlagScore(EnemyFlagScore);
		}
	}
}