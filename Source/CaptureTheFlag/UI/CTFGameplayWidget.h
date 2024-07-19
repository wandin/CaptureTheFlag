// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CTFGameplayWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API UCTFGameplayWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PlayerHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FlagCapturedText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EnemyFlagCapturedText;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;
	
};
