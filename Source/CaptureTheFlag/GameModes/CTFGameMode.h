// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CaptureTheFlag//PlayerController/CTFPlayerController.h"
#include "CaptureTheFlag/PlayerState/CTFPlayerState.h"
#include "GameFramework/GameMode.h"
#include "CTFGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API ACTFGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ACTFGameMode();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	void HandleTeamSelection(AController* PlayerController, int32 TeamID);
	void SetupSpawnPoints();
	void RespawnFlag();
	void PlayerEliminated(ACTFCharacter* ElimmedCharacter, ACTFPlayerController* VictimController, ACTFPlayerController* AttackerController);
	void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void OnTeamScored(ACTFCharacter* ScoringCharacter, ACTFPlayerState* ScoringPlayerState, ACTFPlayerController* ScoringController);

	
private:

	void CheckForGameEnd();

	
	UPROPERTY()
	TArray<AController*> ConnectedPlayers;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> Team1PawnClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> Team2PawnClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACTFFlag> FlagClass;
	
	UPROPERTY(Replicated)
	AActor* Flag;
	
	UPROPERTY()
	TArray<AActor*> Team1SpawnPoints;
	UPROPERTY()
	TArray<AActor*> Team2SpawnPoints;
	UPROPERTY()
	TArray<AActor*> FlagSpawnPoints;


	
	// Use the location of T1SpawnPoint and T2SpawnPoint
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> T1SpawnPoint;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> T2SpawnPoint;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> FlagSpawnAreaClass;
	
	UPROPERTY()
	ACTFPlayerController* WDNPlayerController;
	
};
