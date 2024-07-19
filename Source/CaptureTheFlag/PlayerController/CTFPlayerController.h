// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CaptureTheFlag/Interfaces/PlayerControllerInterface.h"
#include "GameFramework/PlayerController.h"
#include "CaptureTheFlag/Character/CTFCharacter.h"
#include "CTFPlayerController.generated.h"

class ACTFHUD;
enum class EWeaponPoses : uint8;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API ACTFPlayerController : public APlayerController, public IPlayerControllerInterface, public ICharacterInterface
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual ACTFCharacter* GetCharacterRef() override;
	
	void SelectTeam(int32 TeamID);
	UFUNCTION(Server, Reliable)
	void Server_SelectTeam(int32 TeamID);
	void CreateGameplayWidget() const;
	void UpdatePlayerHealth(float Health, float MaxHealth);
	void UpdatePlayerScore(float Score);
	void UpdatePlayerDefeats(int32 Defeats);
	void UpdateFlagScore(int32 FlagPoint);
	void UpdateEnemyFlagScore(int32 EnemyFlagPoint);

private:

	int32 SelectedTeam;
	
	UPROPERTY()
	ACTFHUD* GameHUD;
	UPROPERTY()
	ACTFCharacter* CTFCharacter;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;

	int32 FlagScore;
	int32 EnemyFlagScore;

public:
	
	FORCEINLINE virtual APlayerController* GetPlayerControllerRef() override { return this; }
};
