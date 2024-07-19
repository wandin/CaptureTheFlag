// Fill out your copyright notice in the Description page of Project Settings.


#include "CTFAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CaptureTheFlag//PlayerController/CTFPlayerController.h"
#include "CaptureTheFlag/Character/CTFCharacter.h"

void UCTFAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CTFCharacter = Cast<ACTFCharacter>(TryGetPawnOwner());
}

void UCTFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(!IsValid(CTFCharacter))					/* Check WDNCharacter */
	{
		CTFCharacter = GetCharacterRef();
	}
	if(!IsValid(CTFCharacter)) return;			/* End Check WDNCharacter */
	
	//	Offsets
	CharacterRotation = GetCharacterRotation(CTFCharacter);
	AimPitch = GetCharacterAimPitch(CTFCharacter);
	
	//	Character Speed
	Speed = GetCharacterSpeed(CTFCharacter);
	// Character booleans
	bIsAccelerating = CTFCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsCrouched = CTFCharacter->bIsCrouched;
	bIsFalling = CTFCharacter->GetCharacterMovement()->IsFalling();

	WeaponType = CTFCharacter->EquippedWeaponType;
}

ACTFCharacter* UCTFAnimInstance::GetCharacterRef() const
{
	ACTFCharacter* LocalCharacter = Cast<ACTFCharacter>(TryGetPawnOwner());	
	if(LocalCharacter && LocalCharacter->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		if(ICharacterInterface* CharacterInterface = Cast<ICharacterInterface>(LocalCharacter))
		{
			return CharacterInterface->GetCharacterRef();
		}
	}
	return nullptr;
}

float UCTFAnimInstance::GetCharacterSpeed(const ACTFCharacter* Character)
{
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

float UCTFAnimInstance::GetCharacterRotation(const ACTFCharacter* Character) const
{
	const FRotator AimRotation = Character->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	return UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
}

bool UCTFAnimInstance::GetCharacterIsFalling(const ACTFCharacter* Character)
{
	return Character->GetMovementComponent()->IsFalling();
}

float UCTFAnimInstance::GetCharacterAimPitch(const ACTFCharacter* Character)
{
	return Character->AimPitch;
}
