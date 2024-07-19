// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CTFPlayerState.generated.h"

class ACTFPlayerController;
class ACTFCharacter;
/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API ACTFPlayerState : public APlayerState
{
	GENERATED_BODY()


public:

	ACTFPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	int32 GetTeamID() const;
	void SetTeamID(int32 NewTeamID);

	virtual void OnRep_Score() override;

	UFUNCTION()
	void OnRep_Defeats();

	UFUNCTION()
	void OnRep_FlagScore();

	UFUNCTION()
	void OnRep_EnemyFlagScore();

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	void AddToFlagScore(int32 FlagScoreAmount);
	void AddToEnemyFlagScore(int32 EnemyFlagScoreAmount);
	
private:

	UPROPERTY()
	ACTFCharacter* CTFCharacter;
	
	UPROPERTY()
	ACTFPlayerController* CTFController;
	
	UPROPERTY(Replicated)
	int32 TeamID;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
	
	UPROPERTY(ReplicatedUsing = OnRep_FlagScore)
	int32 FlagScore;

	UPROPERTY(ReplicatedUsing = OnRep_EnemyFlagScore)
	int32 EnemyFlagScore;


public:
	
	FORCEINLINE int32 GetFlagScore() const { return FlagScore; }
	FORCEINLINE int32 GetEnemyFlagScore() const { return EnemyFlagScore; }

	
};
