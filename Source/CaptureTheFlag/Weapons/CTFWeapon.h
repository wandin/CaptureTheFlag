// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CaptureTheFlag/Projectile/CTFProjectile.h"
#include "GameFramework/Actor.h"
#include "CTFWeapon.generated.h"

class UCapsuleComponent;
class ACTFWeapon;

enum class EWeaponTypes : uint8;

USTRUCT()
struct FWeaponProperties
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	FString WeaponName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Type")
	EWeaponTypes WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Class")
	TSubclassOf<ACTFWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Projectile")
	TSubclassOf<ACTFProjectile> ProjectileClass;
};

UCLASS()
class CAPTURETHEFLAG_API ACTFWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ACTFWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "BoxCollision")
	UCapsuleComponent* CapsuleCollision;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	FWeaponProperties WeaponProperties;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Effects")
	USoundBase* FireSound;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFlash;

	void Fire();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayFireEffects();

	
};