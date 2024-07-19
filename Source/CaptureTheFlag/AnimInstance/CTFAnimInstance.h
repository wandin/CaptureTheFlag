// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CaptureTheFlag/Enums/WeaponTypes.h"
#include "CTFAnimInstance.generated.h"

class ACTFCharacter;

enum class EWeaponPoses : uint8;
/**
 * 
 */
UCLASS()
class CAPTURETHEFLAG_API UCTFAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	ACTFCharacter* GetCharacterRef() const;

private:

	//	Character and properties
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	ACTFCharacter* CTFCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CharacterRotation;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AimPitch;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;
	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	EWeaponTypes WeaponType;
	
	static float GetCharacterSpeed(const ACTFCharacter* Character);
	float GetCharacterRotation(const ACTFCharacter* Character) const;
	static bool GetCharacterIsFalling(const ACTFCharacter* Character);

	static float GetCharacterAimPitch(const ACTFCharacter* Character);
};
