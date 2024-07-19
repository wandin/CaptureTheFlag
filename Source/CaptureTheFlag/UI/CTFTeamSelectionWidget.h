// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CTFTeamSelectionWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API UCTFTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	
	
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonTeam1;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonTeam2;

	UFUNCTION()
	void OnTeam1Selected();
	UFUNCTION()
	void OnTeam2Selected();

	
	
};
