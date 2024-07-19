// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFGameMode.h"

#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "CaptureTheFlag/Flag/CTFFlag.h"
#include "CaptureTheFlag/PlayerState/CTFPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "CaptureTheFlag/SpawnArea/CTFSpawnArea.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"


class AGameState;

ACTFGameModeBase::ACTFGameModeBase(): Flag(nullptr), WDNPlayerController(nullptr)
{
	bReplicates = true;
}

void ACTFGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACTFGameModeBase, Flag);
}

void ACTFGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	if(NewPlayer)
	{
		ConnectedPlayers.AddUnique(NewPlayer);
	}
	SetupSpawnPoints();
}

void ACTFGameModeBase::HandleTeamSelection(AController* PlayerController, int32 TeamID)
{
	if (ACTFPlayerController* PC = Cast<ACTFPlayerController>(PlayerController))
	{

		ACTFPlayerState* PS = Cast<ACTFPlayerState>(PC->PlayerState);
		if (PS)
		{
			PS->SetTeamID(TeamID);
		}

		
		FVector SpawnLocation;
		FRotator SpawnRotation;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		if (TeamID == 1 && Team1PawnClass && Team1SpawnPoints.Num() > 0)
		{
			int32 SpawnIndex = FMath::RandRange(0, Team1SpawnPoints.Num() - 1);
			ACTFSpawnArea* SpawnArea = Cast<ACTFSpawnArea>(Team1SpawnPoints[SpawnIndex]);
			if (SpawnArea)
			{
				SpawnLocation = SpawnArea->GetRandomPointInArea();
				SpawnRotation = FRotator::ZeroRotator;
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(Team1PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
				PC->Possess(NewPawn);
			}
		}
		else if (TeamID == 2 && Team2PawnClass && Team2SpawnPoints.Num() > 0)
		{
			int32 SpawnIndex = FMath::RandRange(0, Team2SpawnPoints.Num() - 1);
			ACTFSpawnArea* SpawnArea = Cast<ACTFSpawnArea>(Team2SpawnPoints[SpawnIndex]);
			if (SpawnArea)
			{
				SpawnLocation = SpawnArea->GetRandomPointInArea();
				SpawnRotation = FRotator::ZeroRotator; // Or any desired rotation
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(Team2PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
				PC->Possess(NewPawn);
			}
		}
	}
}

void ACTFGameModeBase::SetupSpawnPoints()
{
	// Clear existing spawn points
	Team1SpawnPoints.Empty();
	Team2SpawnPoints.Empty();
	FlagSpawnPoints.Empty();

	// Get all spawn areas in the world
	TArray<AActor*> AllSpawnAreas;
	UGameplayStatics::GetAllActorsOfClass(this, ACTFSpawnArea::StaticClass(), AllSpawnAreas);

	// Filter spawn areas by team
	for (AActor* SpawnArea : AllSpawnAreas)
	{
		ACTFSpawnArea* SpawnAreaActor = Cast<ACTFSpawnArea>(SpawnArea);
		if (SpawnAreaActor)
		{
			// Logic to differentiate between Team1 and Team2 areas
			// Check if SpawnArea IsA(T1SpawnPoint or T2SpawnPoint) -> both are set in the BP_WDN_GameMode
			if (SpawnAreaActor->IsA(T1SpawnPoint))
			{
				Team1SpawnPoints.Add(SpawnAreaActor);
			}
			else if (SpawnAreaActor->IsA(T2SpawnPoint))
			{
				Team2SpawnPoints.Add(SpawnAreaActor);
			}
			// Logic to identify flag spawn areas
			else if (SpawnAreaActor->IsA(FlagSpawnAreaClass))
			{
				FlagSpawnPoints.Add(SpawnAreaActor);
			}
		}
	}
	// Destroy any existing flag and spawn a new one at the designated spawn point
	if (Flag)
	{
		Flag->Destroy();
		Flag = nullptr;
	}
	
	// Spawn the flag at the designated spawn point
	if (FlagClass && FlagSpawnPoints.Num() > 0)
	{
		FVector FlagSpawnLocation = FlagSpawnPoints[0]->GetActorLocation();
		FRotator FlagSpawnRotation = FlagSpawnPoints[0]->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Flag = GetWorld()->SpawnActor<AActor>(FlagClass, FlagSpawnLocation, FlagSpawnRotation, SpawnParams);
	}
}

void ACTFGameModeBase::RespawnFlag()
{
	if(Flag)
	{
		Flag->Destroy();
		Flag = nullptr;
	}
	
	if (FlagClass && FlagSpawnPoints.Num() > 0)
	{
		FVector FlagSpawnLocation = FlagSpawnPoints[0]->GetActorLocation();
		FRotator FlagSpawnRotation = FlagSpawnPoints[0]->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		Flag = GetWorld()->SpawnActor<ACTFFlag>(FlagClass, FlagSpawnLocation, FlagSpawnRotation, SpawnParams);
	}
}

void ACTFGameModeBase::PlayerEliminated(ACTFCharacter* ElimmedCharacter, ACTFPlayerController* VictimController,
                                       ACTFPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	ACTFPlayerState* AttackerPlayerState = AttackerController ? Cast<ACTFPlayerState>(AttackerController->PlayerState) : nullptr;
	ACTFPlayerState* VictimPlayerState = VictimController ? Cast<ACTFPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	RequestRespawn(ElimmedCharacter, VictimController);
}

void ACTFGameModeBase::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	// Should handle respawning character here using the SpawnArea for its team.

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Destroy();
	}

	if (ElimmedController)
	{
		ACTFPlayerController* PC = Cast<ACTFPlayerController>(ElimmedController);
		if (PC)
		{
			ACTFPlayerState* PS = Cast<ACTFPlayerState>(PC->PlayerState);
			if (PS)
			{
				FVector SpawnLocation;
				FRotator SpawnRotation;
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;

				APawn* NewPawn = nullptr;
				
				if (PS->GetTeamID() == 1 && Team1PawnClass && Team1SpawnPoints.Num() > 0)
				{
					int32 SpawnIndex = FMath::RandRange(0, Team1SpawnPoints.Num() - 1);
					ACTFSpawnArea* SpawnArea = Cast<ACTFSpawnArea>(Team1SpawnPoints[SpawnIndex]);
					if (SpawnArea)
					{
						SpawnLocation = SpawnArea->GetRandomPointInArea();
						SpawnRotation = FRotator::ZeroRotator;
						NewPawn = GetWorld()->SpawnActor<APawn>(Team1PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
						PC->Possess(NewPawn);
					}
				}
				else if (PS->GetTeamID() == 2 && Team2PawnClass && Team2SpawnPoints.Num() > 0)
				{
					int32 SpawnIndex = FMath::RandRange(0, Team2SpawnPoints.Num() - 1);
					ACTFSpawnArea* SpawnArea = Cast<ACTFSpawnArea>(Team2SpawnPoints[SpawnIndex]);
					if (SpawnArea)
					{
						SpawnLocation = SpawnArea->GetRandomPointInArea();
						SpawnRotation = FRotator::ZeroRotator; // Or any desired rotation
						NewPawn = GetWorld()->SpawnActor<APawn>(Team2PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
						PC->Possess(NewPawn);
					}
				}
			}
		}
	}
}

void ACTFGameModeBase::OnTeamScored(ACTFCharacter* ScoringCharacter, ACTFPlayerState* ScoringPlayerState, ACTFPlayerController* ScoringController)
{	
	if (ScoringCharacter == nullptr || ScoringPlayerState == nullptr || ScoringController == nullptr)
		return;

	// Notifica todos os jogadores sobre a atualização da pontuação
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACTFPlayerController* PC = Cast<ACTFPlayerController>(It->Get());
		if (PC && PC->PlayerState)
		{
			ACTFPlayerState* PS = Cast<ACTFPlayerState>(PC->PlayerState);
			if (PS)
			{
				if (PS->GetTeamID() == ScoringPlayerState->GetTeamID())
				{
					PS->AddToFlagScore(1);  // Atualiza o HUD para os jogadores do mesmo time
				}
				else
				{
					PS->AddToEnemyFlagScore(1);  // Atualiza o HUD para os jogadores do time inimigo
				}
			}
		}
	}
	// Respawn flag to original location
	RespawnFlag();

	CheckForGameEnd();
}

void ACTFGameModeBase::CheckForGameEnd()
{
	int32 Team1Score = 0;
	int32 Team2Score = 0;

	// Calcula as pontuações dos times
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACTFPlayerController* PC = Cast<ACTFPlayerController>(It->Get());
		if (PC && PC->PlayerState)
		{
			ACTFPlayerState* PS = Cast<ACTFPlayerState>(PC->PlayerState);
			if (PS)
			{
				if (PS->GetTeamID() == 1)
				{
					Team1Score = PS->GetFlagScore();
				}
				else if (PS->GetTeamID() == 2)
				{
					Team2Score = PS->GetFlagScore();
				}
			}
		}
	}

	// Reinicia o jogo se algum time alcançar 3 pontos
	if (Team1Score >= 3 || Team2Score >= 3)
	{
		// Should Restart The Game here
		RestartGame();
	}
}
